/**
 * @file LibraryService.cpp
 * @brief Implements library scanning, filtering, and favourite operations.
 */

#include "library/LibraryService.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QString>
#include <set>
#include <algorithm>
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QPointer>

/**
 * @brief Determines whether a file has a supported ROM extension.
 *
 * @param filePath Absolute path to a file discovered while scanning.
 * @return `true` if the file is a supported ROM, otherwise `false`.
 */
static bool isSupportedRomFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "nes"|| extension == "sfc"|| extension == "smc"|| extension == "gba"|| 
        extension == "gb"|| extension == "gbc"|| extension == "n64"|| extension == "z64") {
        return true;
    }

    return false;
}


/**
 * @brief Builds a duplicate-detection key from a ROM file's name and extension.
 *
 * @param fileInfo Metadata for a discovered ROM file.
 * @return Normalized key used to suppress duplicates during a scan.
 */
static std::string buildDuplicateKey(const QFileInfo& fileInfo) {
    QString title = fileInfo.completeBaseName().toLower().trimmed();
    QString extension = fileInfo.suffix().toLower().trimmed();

    QString dupKey = title + '|' + extension;
    return dupKey.toStdString();
    ;
}


/**
 * @brief Builds a deterministic identifier for a game.
 *
 * @param nonDupKey Duplicate-suppression key derived from the ROM file.
 * @return Stable SHA-1 based identifier for the game.
 */
static std::string buildStableGameId(const std::string& nonDupKey) {
    QByteArray hash = QCryptographicHash::hash(
        QByteArray::fromStdString(nonDupKey), 
        QCryptographicHash::Sha1
    );

    std::string gameId = hash.toHex().toStdString();
    return gameId;
}


/**
 * @brief Checks whether a game title matches the current search query.
 *
 * @param game Game being evaluated.
 * @param searchTitle Case-insensitive title fragment entered by the user.
 * @return `true` when the game should remain in the filtered result.
 */
static bool titleMatchGiven(const Game& game, const std::string& searchTitle) {
    if(searchTitle.empty() == true) {
        return true;
    }
    
    QString gameTitle = QString::fromStdString(game.title).trimmed().toLower();
    QString loweredQuery = QString::fromStdString(searchTitle).trimmed().toLower();

    return gameTitle.contains(loweredQuery);
}


/**
 * @brief Checks whether a game passes the favourite-only filter.
 *
 * @param game Game being evaluated.
 * @param favouriteOnly Whether only favourites should be included.
 * @return `true` when the game should remain in the filtered result.
 */
static bool checkFavouriteFilter(const Game& game, bool favouriteOnly) {
    if(favouriteOnly == false) {
        return true;
    }

    return game.favourite == true;
}


static std::string getSystemFromExtension(const QFileInfo& fileInfo) {
     QString extension = fileInfo.suffix().trimmed().toLower();

     if(extension == "nes") {
        return "NES";
    }

    if(extension == "sfc" || extension == "smc") {
        return "Super NES";
    }

    if(extension == "gba") {
        return "Game Boy Advance";
    }

    if(extension == "gb") {
        return "Game Boy";
    }

    if(extension == "gbc") {
        return "Game Boy Color";
    }

    if(extension == "n64" || extension == "z64") {
        return "Nintendo 64";
    }

    return "Unknown";
}


/**
 * @brief Constructs the library service using the given repository implementation.
 *
 * @param library Repository that persists and retrieves games.
 * @param emulatorManager Manager used by the game launcher.
 * @param parent Optional parent QObject.
 */
LibraryService::LibraryService(ILibrary& library, EmulatorManager& emulatorManager, QObject* parent)
    : QObject(parent), library(library), gameLauncher(emulatorManager) {
}


/**
 * @brief Scans configured ROM folders and syncs the stored library to disk contents.
 *
 * @param settings Application settings containing ROM directories to traverse.
 * @return Summary of the scan work and any issues encountered.
 */
ScanResult LibraryService::scanLibrary(const Settings& settings) {
    ScanResult result;
    std::set<std::string> foundPaths;
    std::set<std::string> addedKeys;

    for(const std::string& directory : settings.romDirectories) {
        QString folderPath = QString::fromStdString(directory);

        QDir romFolder(folderPath);
        if(romFolder.exists() == false) {
            result.errors.push_back("ROM folder could not be accessed or may be missing: " + directory);
            continue;
        }

        result.foldersScanned++;
        QDirIterator romIterator(folderPath, QDir::Files, QDirIterator::Subdirectories);
        
        while(romIterator.hasNext() == true) {
            QString filePath = romIterator.next();

            if(isSupportedRomFile(filePath) == false) {
                continue;
            }

            QFileInfo fileInfo(filePath);
            std::string dupKey = buildDuplicateKey(fileInfo);

            if(addedKeys.find(dupKey) != addedKeys.end()) {
                continue;
            }

            Game game;
            game.id = buildStableGameId(dupKey);
            game.title = fileInfo.completeBaseName().toStdString();
            game.romPath = QDir::cleanPath(filePath).toStdString();
            game.system = getSystemFromExtension(fileInfo);

            addedKeys.insert(dupKey);
            foundPaths.insert(game.romPath);
            
            library.updateOrInsert(game);
            result.gamesFound++;
        }
    }

    std::vector<Game> allGames = library.getAllGames();

    for(const Game& game : allGames) {
        if(foundPaths.find(game.romPath) == foundPaths.end()) {
            library.removeGame(game.id);
        }
    }
    
    return result;
}


/**
 * @brief Returns the full stored game library.
 *
 * @return Collection of all persisted games.
 */
std::vector<Game> LibraryService::getLibrary() {
    return library.getAllGames();
}


/**
 * @brief Retrieves detailed information for a single game.
 *
 * @param id Stable game identifier.
 * @return Game record, or an empty game if the id is unknown.
 */
Game LibraryService::getGameDetails(const std::string& id) {
    return library.getGame(id);
}


/**
 * @brief Toggles the favourite state of the specified game.
 *
 * @param id Stable identifier of the game to flip.
 */
void LibraryService::toggleFavourite(const std::string& id) {
    Game game = library.getGame(id);
    if(game.id.empty() == true) {
        return;
    }

    library.setFavourite(id, game.favourite == false);
}


/**
 * @brief Launches a game and updates play tracking after the emulator exits.
 *
 * @param id Stable game identifier.
 * @param settings Current settings used to find the emulator.
 * @return Launch result from the launcher.
 */
LaunchResult LibraryService::launchGame(const std::string& id, const Settings& settings) {
    Game game = library.getGame(id);
    LaunchResult result = gameLauncher.launch(game, settings);

    if(result.success == true) {
        QDateTime startTime = QDateTime::currentDateTime();
        QPointer<QProcess> guardedProcess(result.process);

        QTimer::singleShot(2000, this, [this, id, guardedProcess]() {
            if(guardedProcess != nullptr && guardedProcess -> state() != QProcess::NotRunning) {
                library.updateLastPlayed(id);
            }
        });

        QObject::connect(result.process, &QProcess::finished, this,
            [this, id, startTime, gameTitle = QString::fromStdString(game.title), guardedProcess]
            (int exitCode, QProcess::ExitStatus exitStatus) {
                int elapsed = startTime.secsTo(QDateTime::currentDateTime());

                if (elapsed > 0) { 
                    library.updateTimePlayed(id, elapsed);
                }

                bool quitTooFast = elapsed < 2;
                bool crashed = (exitStatus == QProcess::CrashExit);
                bool failedExit = (exitCode != 0);

                if(quitTooFast == true && (crashed == true || failedExit == true)) {
                    emit gameLaunchFailed(
                        QString("The emulator opened but \"%1\" did not launch correctly. "
                                "Check the ROM file and launch settings, then try again.")
                            .arg(gameTitle)
                    );
                }

                emit gameLaunchFinished();
                if(guardedProcess != nullptr) {
                    guardedProcess->deleteLater();
                }
        });
    }



    return result;
 }


/**
 * @brief Returns games filtered and sorted according to the active UI controls.
 *
 * @param searchTitle Case-insensitive fragment that must appear in the title.
 * @param favouriteOnly Whether to include only favourited games.
 * @param sortMode Title sort direction.
 * @return Filtered and sorted games.
 */
std::vector<Game> LibraryService::getFilteredLibrary(const std::string& searchTitle,bool favouriteOnly, SortMode sortMode) {
    std::vector<Game> allGames = library.getAllGames();
    std::vector<Game> filteredGames;

    for(const Game& game : allGames) {
        if(titleMatchGiven(game, searchTitle) == false) {
            continue;
        }

        if(checkFavouriteFilter(game, favouriteOnly) == false) {
            continue;
        }

        filteredGames.push_back(game);
    }

    std::sort(filteredGames.begin(), filteredGames.end(), [&](const Game& left, const Game& right) {
        QString leftTitle = QString::fromStdString(left.title).trimmed().toLower();
        QString rightTitle = QString::fromStdString(right.title).trimmed().toLower();

        if(leftTitle == rightTitle) {
            return left.id < right.id;
        }

        if(sortMode == SortMode::TitleAscending) {
            return leftTitle < rightTitle;
        }

        return leftTitle > rightTitle;
    });


    return filteredGames;
}

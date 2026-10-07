/**
 * @file GameLauncher.cpp
 * @brief Implements checks and startup logic for launching games.
 */
#include "library/GameLauncher.h"

#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QFile>
#include <QByteArray>

/**
 * @brief Gets a simple minimum ROM size for a system.
 *
 * @param system Console system name.
 * @return Smallest size that seems reasonable for that system.
 */
static qint64 minimumRomSizeForSystem(const std::string& system) {
    if(system == "NES") {
        return 16;
    }

    if(system == "Game Boy" || system == "Game Boy Color") {
        return 512;
    }

    if(system == "Game Boy Advance") {
        return 1024;
    }

    if(system == "Nintendo 64") {
        return 4096;
    }

    if(system == "Super NES") {
        return 1024;
    }

    return 256;
}


/**
 * @brief Checks if the first part of a file looks like text instead of ROM data.
 *
 * @param data Bytes read from the start of the file.
 * @return `true` when the data looks like normal text.
 */
static bool looksLikePlainText(const QByteArray& data) {
    if(data.isEmpty() == true) {
        return false;
    }

    int printableCount = 0;
    int newlineCount = 0;
    int zeroByteCount = 0;

    for(char ch : data) {
        unsigned char value = static_cast<unsigned char>(ch);

        if(value == 0) {
            zeroByteCount++;
        }

        if(value == '\n' || value == '\r' || value == '\t') {
            printableCount++;
        }
        else if(value >= 32 && value <= 126) {
            printableCount++;
        }

        if(value == '\n' || value == '\r') {
            newlineCount++;
        }
    }

    double printableRatio = static_cast<double>(printableCount) / static_cast<double>(data.size());

    if(zeroByteCount > 0) {
        return false;
    }

    return printableRatio > 0.90 && newlineCount > 0;
}


/**
 * @brief Makes sure a ROM file is readable and looks valid enough to launch.
 *
 * @param game Game whose ROM file is being checked.
 * @param errorMessage Output message filled when the ROM does not look valid.
 * @return `true` if the ROM passes the basic checks.
 */
static bool isRomFilePlausible(const Game& game, QString& errorMessage) {
    QFile romFile(QString::fromStdString(game.romPath));

    if(romFile.open(QIODevice::ReadOnly) == false) {
        errorMessage = "ROM file could not be opened. Check that the file still exists and can be read.";
        return false;
    }

    qint64 fileSize = romFile.size();
    qint64 minimumSize = minimumRomSizeForSystem(game.system);

    if(fileSize <= 0) {
        errorMessage = "ROM file is empty. Rescan your ROM folders or replace the invalid file.";
        
        return false;
    }

    if(fileSize < minimumSize) {
        errorMessage = QString("ROM file looks invalid for %1 because it is too small to be a usable game file.")
            .arg(QString::fromStdString(game.system));
        
            return false;
    }

    QByteArray firstChunk = romFile.read(512);

    if(looksLikePlainText(firstChunk) == true) {
        errorMessage = QString("ROM file looks like plain text instead of a valid %1 game file. "
                               "Replace the file and rescan your ROM folders.")
            .arg(QString::fromStdString(game.system));
        
            return false;
    }

    return true;
}

/**
 * @brief Creates a launcher that uses the given emulator manager.
 *
 * @param emulatorManager Manager used to find emulator templates.
 */
GameLauncher::GameLauncher(EmulatorManager& emulatorManager)
    : emulatorManager(emulatorManager) {
}


/**
 * @brief Validates the selected game and starts its emulator process.
 *
 * @param game Game selected by the user.
 * @param settings Current application settings.
 * @return Launch result that says if startup worked.
 */
LaunchResult GameLauncher::launch(const Game& game, const Settings& settings) {
    if(game.id.empty() == true) {
        return {false, "The selected game could not be found in the library."};
    }

    QFileInfo romFile(QString::fromStdString(game.romPath));
    if(romFile.exists() == false || romFile.isFile() == false) {
        return {
            false,
            std::string("ROM file is missing for \"") + game.title +
            "\". Rescan your ROM folders or remove the missing entry from the library."
        };
    }

    QString romValidationError;
    if(isRomFilePlausible(game, romValidationError) == false) {
        return {false, romValidationError.toStdString()};
    }

    const EmulatorTemplate* emulatorTemplate = emulatorManager.findTemplate(settings, game.system);
    if(emulatorTemplate == nullptr) {
        return {
            false,
            "Invalid emulator configuration for " + game.system +
            ". Open Settings and create an emulator path for this system."
        };
    }

    if(emulatorTemplate -> enabled == false) {
        return {
            false,
            "The emulator configuration for " + game.system +
            " is disabled. Open Settings and enable or fix this system before launching."
        };
    }

    EmulatorStatus status = emulatorManager.getStatus(emulatorTemplate);
    if(status == EmulatorStatus::NOTINSTALLED) {
        return {
            false,
            "No emulator is configured for " + game.system +
            ". Open Settings and set an emulator path for this system."
        };
    }

    if(status == EmulatorStatus::MISSING) {
        return {
            false,
            "The configured emulator for " + game.system +
            " could not be found. Open Settings and update the emulator path."
        };
    }

    

    QString emulator = QString::fromStdString(emulatorTemplate -> emulatorPath);

    QString argumentsText = QString::fromStdString(emulatorTemplate -> arguments).trimmed();
    QStringList arguments;
    
    if(argumentsText.isEmpty() == false) {
        arguments = QProcess::splitCommand(argumentsText);
    }

    arguments << QString::fromStdString(game.romPath);

    QProcess* process = new QProcess();
    process->start(emulator, arguments);
    if (!process->waitForStarted(6000)) {
        delete process;
        return {
            false,
            "Emulator failed to start for " + game.system +
            ". Check the emulator path and launch settings, then try again."
        };
    }

    return {true, "Launch successful", process};
}

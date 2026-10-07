/**
 * @file LibraryService.h
 * @brief Declares business logic for scanning and exposing the game library.
 */
#pragma once

#include "ILibrary.h"
#include "core/Settings.h"
#include "library/GameLauncher.h"

#include <vector>
#include <string>
#include <QString>

/**
 * @brief Summarizes the outcome of a library scan.
 */
struct ScanResult {
    int foldersScanned = 0;            ///< Number of configured folders that were successfully traversed.
    int gamesFound = 0;                ///< Number of supported games discovered during the scan.
    std::vector<std::string> errors;   ///< Non-fatal issues encountered while scanning folders.
};

/**
 * @brief Defines the supported sort orders for the visible game list.
 */
enum class SortMode {
    TitleAscending,  ///< Sort games from A to Z.
    TitleDescending  ///< Sort games from Z to A.
};

/**
 * @brief Applies library business rules on top of the storage layer.
 */
class LibraryService : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Creates a service that uses the supplied library repository.
     *
     * @param library Repository used for all game persistence operations.
     * @param emulatorManager Manager used to launch games with emulators.
     * @param parent Optional parent QObject.
     */
    explicit LibraryService(ILibrary& library, EmulatorManager& emulatorManager,  QObject* parent = nullptr);

    /**
     * @brief Scans configured ROM folders and synchronizes the stored library.
     *
     * @param settings Application settings that define which ROM folders to scan.
     * @return Summary of the scan results.
     */
    ScanResult scanLibrary(const Settings& settings);

    /**
     * @brief Returns every game in the stored library.
     *
     * @return Collection of stored games.
     */
    std::vector<Game> getLibrary();

    /**
     * @brief Retrieves a single game's details.
     *
     * @param id Stable game identifier.
     * @return Matching game record, or an empty game if not found.
     */
    Game getGameDetails(const std::string& id);

    /**
     * @brief Inverts the favourite state for a game.
     *
     * @param id Stable identifier of the game to update.
     */
    void toggleFavourite(const std::string& id);

    /**
     * @brief Launches one game and tracks play time when it exits.
     *
     * @param id Stable identifier of the game to launch.
     * @param settings Current app settings with emulator paths.
     * @return Launch result from the game launcher.
     */
    LaunchResult launchGame(const std::string& id, const Settings& settings);

    /**
     * @brief Returns the library filtered by search text, favourites, and sort order.
     *
     * @param searchTitle Case-insensitive title fragment to match.
     * @param favouriteOnly Whether only favourite games should be included.
     * @param sortMode Requested title sort direction.
     * @return Filtered and sorted collection of games.
     */
    std::vector<Game> getFilteredLibrary(const std::string& searchTitle, bool favouriteOnly, SortMode sortMode);

signals:
    /**
     * @brief Emitted when a launched emulator process finishes normally.
     */
    void gameLaunchFinished();

    /**
     * @brief Emitted when a launched emulator closes too quickly or fails.
     *
     * @param message Error message to show the user.
     */
    void gameLaunchFailed(const QString& message);

private:
    ILibrary& library;
    GameLauncher gameLauncher;
};

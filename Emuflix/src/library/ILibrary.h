/**
 * @file ILibrary.h
 * @brief Interface for game library persistence operations.
 */
#pragma once

#include "core/Game.h"

#include <vector>
#include <string>

/**
 * @brief Defines the persistence operations required by the library service.
 */
class ILibrary {

public:
    virtual ~ILibrary() = default;

    /**
     * @brief Retrieves every game currently stored by the repository.
     *
     * @return Collection of stored games.
     */
    virtual std::vector<Game> getAllGames() = 0;

    /**
     * @brief Retrieves a single game by identifier.
     *
     * @param id Stable game identifier.
     * @return Matching game record, or an empty game if no record exists.
     */
    virtual Game getGame(const std::string& id) = 0;

    /**
     * @brief Inserts a game or updates the existing stored record.
     *
     * @param game Game record to persist.
     */
    virtual void updateOrInsert(const Game& game) = 0;

    /**
     * @brief Removes a game from storage.
     *
     * @param id Stable identifier of the game to remove.
     */
    virtual void removeGame(const std::string& id) = 0;

    /**
     * @brief Persists the favourite state for a stored game.
     *
     * @param id Stable identifier of the game to update.
     * @param favourite New favourite state.
     */
    virtual void setFavourite(const std::string& id, bool favourite) = 0;

    /**
     * @brief Updates the last played time for a stored game.
     *
     * @param id Stable identifier of the game to update.
     */
    virtual void updateLastPlayed(const std::string& id) = 0;

    /**
     * @brief Adds play time to a stored game.
     *
     * @param id Stable identifier of the game to update.
     * @param seconds Number of seconds to add.
     */
    virtual void updateTimePlayed(const std::string& id, int seconds) = 0;
};

/**
 * @file SQLiteLibrary.h
 * @brief Declares the SQLite-backed game library repository.
 */
#pragma once

#include "library/ILibrary.h"
#include "data/SQLiteDatabase.h"

/**
 * @brief SQLite-backed implementation of the game library repository.
 */
class SQLiteLibrary :public ILibrary {

public:
    /**
     * @brief Creates a repository that uses the supplied database connection.
     *
     * @param db Database wrapper that owns the active SQLite connection.
     */
    explicit SQLiteLibrary(SQLiteDatabase& db);

    /**
     * @brief Loads every stored game ordered by title.
     *
     * @return Collection of games currently stored in the database.
     */
    std::vector<Game> getAllGames() override;

    /**
     * @brief Looks up a single game by identifier.
     *
     * @param id Stable game identifier.
     * @return Matching game record, or an empty game if not found.
     */
    Game getGame(const std::string& id) override;

    /**
     * @brief Updates an existing game or inserts it if it does not exist yet.
     *
     * @param game Game record to persist.
     */
    void updateOrInsert(const Game& game) override;

    /**
     * @brief Deletes a game from storage.
     *
     * @param id Stable identifier of the game to remove.
     */
    void removeGame(const std::string& id) override;

    /**
     * @brief Stores the favourite state for a game.
     *
     * @param id Stable identifier of the game to update.
     * @param favourite New favourite state to persist.
     */
    void setFavourite(const std::string& id, bool favourite) override;

    /**
     * @brief Stores the current time as the last played time for a game.
     *
     * @param id Stable identifier of the game to update.
     */
    void updateLastPlayed(const std::string& id) override;

    /**
     * @brief Adds played seconds to a game's total play time.
     *
     * @param id Stable identifier of the game to update.
     * @param seconds Number of seconds to add.
     */
    void updateTimePlayed(const std::string& id, int seconds) override;

private:
    SQLiteDatabase& db;
};

/**
 * @file SQLiteLibrary.cpp
 * @brief Implements the SQLite-backed game library repository.
 */

#include "data/SQLiteLibrary.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include <QVariant>
#include <QDateTime>

/**
 * @brief Converts the current SQL row into a `Game` value object.
 *
 * @param query Query positioned on a valid row.
 * @return Game populated from the current result row.
 */
static Game queryForGame(const QSqlQuery& query) {
    return {
        query.value("id").toString().toStdString(),
        query.value("name").toString().toStdString(),
        query.value("romPath").toString().toStdString(),
        query.value("favourite").toInt() != 0,
        query.value("lastPlayed").toString().toStdString(),
        query.value("timePlayed").toInt(),
        query.value("system").toString().toStdString()
    };
}


/**
 * @brief Constructs the SQLite-backed library repository.
 *
 * @param db Database wrapper that owns the active SQLite connection.
 */
SQLiteLibrary::SQLiteLibrary(SQLiteDatabase& db)
    :db(db) {
}


/**
 * @brief Retrieves all stored games ordered by title.
 *
 * @return Collection of games currently stored in the database.
 */
std::vector<Game> SQLiteLibrary::getAllGames() {
    std::vector<Game> allGames;

    QSqlDatabase& database = db.getDatabase();
    if(database.isOpen() == false) {
        return allGames;
    }

    QSqlQuery gameQuery(database);
    gameQuery.prepare(
        "SELECT games.id, games.name, games.romPath, games.lastPlayed, games.timePlayed, games.system, "
        "COALESCE(game_settings.favourite, 0) AS favourite "
        "From games "
        "LEFT JOIN game_settings ON game_settings.gameID = games.id "
        "ORDER BY games.name COLLATE NOCASE ASC, games.id ASC"
    );

    if(gameQuery.exec() == false) {
        return allGames;
    }

    while(gameQuery.next() == true) {
        allGames.push_back(queryForGame(gameQuery));
    }

    return allGames;
}


/**
 * @brief Retrieves a single game by identifier.
 *
 * @param id Stable game identifier.
 * @return Matching game record, or an empty game when missing.
 */
Game SQLiteLibrary::getGame(const std::string& id) {
    QSqlDatabase& database = db.getDatabase();
    if(database.isOpen() == false) {
        return {"", "", "", false, "", 0, ""};
    }

    QSqlQuery gameQuery(database);
    gameQuery.prepare(
        "SELECT games.id, games.name, games.romPath, games.lastPlayed, games.timePlayed, games.system, "
        "COALESCE(game_settings.favourite, 0) AS favourite "
        "From games "
        "LEFT JOIN game_settings ON game_settings.gameID = games.id "
        "WHERE games.id = :id "
        "LIMIT 1"
    );

    gameQuery.bindValue(":id", QString::fromStdString(id));
    if(gameQuery.exec() == false || gameQuery.next() == false) {
        return {"", "", "", false, "", 0, ""};
    }

    return queryForGame(gameQuery);
}


/**
 * @brief Updates an existing game or inserts it if it does not exist.
 *
 * @param game Game record to persist.
 */
void SQLiteLibrary::updateOrInsert(const Game& game) {
    if(game.id.empty() == true || game.title.empty() == true || game.romPath.empty() == true) {
        return;
    }

    QSqlDatabase& database = db.getDatabase();
    if(database.isOpen() == false) return;
    if (database.transaction() == false) return;
    
    const QString gameId = QString::fromStdString(game.id);
    const QString gameTitle = QString::fromStdString(game.title);
    const QString romPath = QString::fromStdString(game.romPath);
    const QString system = QString::fromStdString(game.system);
    
    QSqlQuery updateQuery(database);
    updateQuery.prepare(
        "UPDATE games "
        "SET name = :name, romPath = :romPath, system = :system, lastSeen = datetime('now') "
        "WHERE id = :id"
    );
    
    updateQuery.bindValue(":id", gameId);
    updateQuery.bindValue(":name", gameTitle);
    updateQuery.bindValue(":romPath", romPath);
    updateQuery.bindValue(":system", system);

    if(updateQuery.exec() == false) {
        database.rollback();
        return;
    }

    if(updateQuery.numRowsAffected() == 0) {
        QSqlQuery romUpdateQuery(database);
        romUpdateQuery.prepare(
            "UPDATE games "
            "SET name = :name, romPath = :romPath, system = :system, lastSeen = datetime('now') "
            "WHERE romPath = :romPath"
        );

        romUpdateQuery.bindValue(":id", gameId);
        romUpdateQuery.bindValue(":name", gameTitle);
        romUpdateQuery.bindValue(":romPath", romPath);
        romUpdateQuery.bindValue(":system", system);

        if(romUpdateQuery.exec() == false) {
            database.rollback();
            return;
        }

        if(romUpdateQuery.numRowsAffected() == 0){
            QSqlQuery insertQuery(database);
            insertQuery.prepare(
                "INSERT INTO games (id, name, romPath, system, lastSeen) "
                "VALUES (:id, :name, :romPath, :system, datetime('now'))"
            );

            insertQuery.bindValue(":id", gameId);
            insertQuery.bindValue(":name", gameTitle);
            insertQuery.bindValue(":romPath", romPath);
            insertQuery.bindValue(":system", system);

            if(insertQuery.exec() == false) {
                database.rollback();
                return;
            }
        }
        
    }

    if(database.commit() == false) {
        database.rollback();
    }
}


/**
 * @brief Deletes a game from storage.
 *
 * @param id Stable identifier of the game to remove.
 */
void SQLiteLibrary::removeGame(const std::string& id) {
    QSqlDatabase& database = db.getDatabase();

    if(database.isOpen() == false) {
        return;
    }

    QSqlQuery removeQuery(database);
    removeQuery.prepare(
        "DELETE FROM games "
        "WHERE id = :id"
    );

    removeQuery.bindValue(":id", QString::fromStdString(id));
    removeQuery.exec();
}


/**
 * @brief Persists the favourite state for a game.
 *
 * @param id Stable identifier of the game to update.
 * @param favourite New favourite state to store.
 */
void SQLiteLibrary::setFavourite(const std::string& id, bool favourite) {
    QSqlDatabase& database = db.getDatabase();

    if(database.isOpen() == false) {
        return;
    }

    QSqlQuery favQuery(database);
    favQuery.prepare(
        "INSERT INTO game_settings(gameID, favourite) "
        "VALUES(:id, :favourite) "
        "ON CONFLICT(gameID) DO UPDATE SET favourite = excluded.favourite"
    );

    favQuery.bindValue(":id", QString::fromStdString(id));
    favQuery.bindValue(":favourite", favourite == true ? 1 : 0);
    favQuery.exec();
}

/**
 * @brief Updates a game's last played timestamp to the current time.
 *
 * @param id Identifier of the game.
 */
void SQLiteLibrary::updateLastPlayed(const std::string& id) {
    QSqlDatabase& database = db.getDatabase();

    if(database.isOpen() == false) {
        return;
    }
    QString localCurrentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss AP t");
    
    QSqlQuery updateQuery(database);
    updateQuery.prepare(
        "UPDATE games "
        "SET lastPlayed = :lastPlayed "
        "WHERE id = :id"
    );

    updateQuery.bindValue(":lastPlayed", localCurrentTime);
    updateQuery.bindValue(":id", QString::fromStdString(id));
    updateQuery.exec();
}


/**
 * @brief Updates the time played in a game.
 * 
 * @param id Identifier of the game.
 * @param seconds Time passed since game was open, updates on a += basis.
 */
void SQLiteLibrary::updateTimePlayed(const std::string& id, int seconds) {
    QSqlDatabase& database = db.getDatabase();
    if(database.isOpen() == false) return;

    QSqlQuery updateQuery(database);
    updateQuery.prepare(
        "UPDATE games "
        "SET timePlayed = timePlayed + :seconds "
        "WHERE id = :id"
    );

    updateQuery.bindValue(":seconds", seconds);
    updateQuery.bindValue(":id", QString::fromStdString(id));
    updateQuery.exec();
}

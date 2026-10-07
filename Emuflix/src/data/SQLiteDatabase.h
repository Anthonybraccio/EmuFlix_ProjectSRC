/**
 * @file SQLiteDatabase.h
 * @brief Declares the SQLite connection wrapper used by the data layer.
 */
#pragma once

#include <QString>
#include <QtSql/QSqlDatabase>

/**
 * @brief Owns a single SQLite connection and initializes the local database schema.
 */
class SQLiteDatabase {

public:
    /**
     * @brief Creates a uniquely named SQLite connection wrapper.
     */
    SQLiteDatabase();

    /**
     * @brief Closes and unregisters the SQLite connection.
     */
    ~SQLiteDatabase();

    SQLiteDatabase(const SQLiteDatabase& other) = delete;
    SQLiteDatabase& operator=(const SQLiteDatabase& other) = delete;

    /**
     * @brief Opens the SQLite database file for this connection.
     *
     * @param filePath Path to the SQLite database file.
     * @param errOutput Optional destination for a human-readable error message.
     * @return `true` if the database was opened successfully, otherwise `false`.
     */
    bool open(const QString& filePath, QString* errOutput = nullptr);

    /**
     * @brief Executes the schema script bundled with the application.
     *
     * @param errOutput Optional destination for a human-readable error message.
     * @return `true` if the schema was applied successfully, otherwise `false`.
     */
    bool initializeSchema(QString* errOutput = nullptr);

    /**
     * @brief Exposes the active Qt SQL database connection.
     *
     * @return Reference to the managed `QSqlDatabase` instance.
     */
    QSqlDatabase& getDatabase();

private: 
    QString uniqueName;
    QSqlDatabase db;
};

/**
 * @file DatabasePath.h
 * @brief Declares helpers for resolving the application's SQLite file path.
 */
#pragma once

#include <QString>

/**
 * @brief Resolves the on-disk location used for the application's SQLite database.
 */
class DatabasePath {

public:
    /**
     * @brief Returns the database file path, creating the application data folder if needed.
     *
     * @return Absolute path to the SQLite database file.
     */
    static QString getDatabaseFilePath();
};

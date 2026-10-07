/**
 * @file SQLiteSettings.h
 * @brief Declares the SQLite-backed settings repository.
 */
#pragma once

#include "settings/ISettings.h"
#include "data/SQLiteDatabase.h"
#include "library/EmulatorManager.h"

/**
 * @brief SQLite-backed implementation of the settings repository.
 */
class SQLiteSettings : public ISettings {

public:
    /**
     * @brief Creates a settings repository that uses the supplied database connection.
     *
     * @param db Database wrapper that owns the active SQLite connection.
     */
    SQLiteSettings(SQLiteDatabase& db, EmulatorManager& emulatorManager);

    /**
     * @brief Loads persisted settings from the database.
     *
     * @return Current application settings.
     */
    Settings load() override;

    /**
     * @brief Replaces the persisted settings with a new snapshot.
     *
     * @param settings Settings values to store.
     */
    void save(const Settings& settings) override;

private:
    SQLiteDatabase& db;
    EmulatorManager& emulatorManager;
};

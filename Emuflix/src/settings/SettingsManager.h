/**
 * @file SettingsManager.h
 * @brief Declares the application-facing settings manager.
 */
#pragma once 

#include "settings/ISettings.h"

/**
 * @brief Provides a thin application-facing wrapper around the settings repository.
 */
class SettingsManager {

public:
    /**
     * @brief Creates a manager backed by the supplied settings store.
     *
     * @param settingsStore Repository used to load and save settings.
     */
    explicit SettingsManager(ISettings& settingsStore);

    /**
     * @brief Loads the latest saved settings.
     *
     * @return Persisted application settings.
     */
    Settings load();

    /**
     * @brief Saves a new settings snapshot.
     *
     * @param settings Settings values to persist.
     */
    void save(const Settings& settings);

private:
    ISettings& store;
};

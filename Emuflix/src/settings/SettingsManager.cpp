/**
 * @file SettingsManager.cpp
 * @brief Implements the application-facing settings manager wrapper.
 */

#include "settings/SettingsManager.h"

/**
 * @brief Constructs a manager that forwards to the given settings store.
 *
 * @param settingsStore Repository that persists settings data.
 */
SettingsManager::SettingsManager(ISettings& settingsStore) 
    : store(settingsStore) {
}


/**
 * @brief Loads the latest persisted settings.
 *
 * @return Current settings snapshot.
 */
Settings SettingsManager::load() {
    return store.load();
}


/**
 * @brief Saves the provided settings snapshot.
 *
 * @param settings Settings values to persist.
 */
void SettingsManager::save(const Settings& settings) {
    store.save(settings);
}

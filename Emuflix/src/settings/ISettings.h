/**
 * @file ISettings.h
 * @brief Interface for settings persistence operations.
 */
#pragma once

#include "core/Settings.h"

/**
 * @brief Defines the persistence operations required for application settings.
 */
class ISettings {

public:
    virtual ~ISettings() = default;

    /**
     * @brief Loads the current settings snapshot.
     *
     * @return Persisted application settings.
     */
    virtual Settings load() = 0;

    /**
     * @brief Saves a new settings snapshot.
     *
     * @param settings Settings values to persist.
     */
    virtual void save(const Settings& settings) = 0;
};

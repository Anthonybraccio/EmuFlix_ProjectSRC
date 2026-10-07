/**
 * @file Settings.h
 * @brief Declares the Settings value object.
 */
#pragma once

#include "core/EmulatorTemplate.h"

#include <string>
#include <vector>

/**
 * @brief Stores the user-configurable application settings.
 */
struct Settings {
    std::vector<std::string> romDirectories; ///< Folders scanned for supported ROM files.
    std::vector<EmulatorTemplate> emulatorTemplates;

    bool setupCompleted = false;
    std::vector<std::string> selectedInstallSystems;
};

/**
 * @file EmulatorTemplate.h
 * @brief Stores emulator settings for one game system.
 */
#pragma once
#include <string>

/**
 * @brief Settings used to launch games for one console system.
 */
struct EmulatorTemplate {
    std::string system;       ///< Name of the console system.
    std::string emulatorPath; ///< Path to the emulator executable.
    std::string arguments;    ///< Extra command line arguments for the emulator.
    bool enabled = true;      ///< Whether this emulator setup can be used.
};

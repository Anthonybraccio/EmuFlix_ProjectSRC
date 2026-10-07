/**
 * @file EmulatorStatus.h
 * @brief Defines the setup state of an emulator.
 */
#pragma once

/**
 * @brief Shows whether an emulator is ready for launching games.
 */
enum class EmulatorStatus {
    NOTINSTALLED, ///< No emulator path has been set.
    MISSING,      ///< A path exists, but the emulator file is not valid.
    READY         ///< The emulator looks ready to use.
};

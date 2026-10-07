/**
 * @file LaunchResult.h
 * @brief Stores the result from trying to start a game.
 */
#pragma once
#include <string>
#include <QProcess>

/**
 * @brief Tells the app if a game launch worked and keeps the running process.
 */
struct LaunchResult {
    bool success = false;     ///< True when the emulator started successfully.
    std::string message;      ///< Message explaining the launch result.
    QProcess* process = nullptr; ///< Running emulator process, when one was started.
};

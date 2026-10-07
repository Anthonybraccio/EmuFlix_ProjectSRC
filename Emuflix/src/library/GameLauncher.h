/**
 * @file GameLauncher.h
 * @brief Declares the helper that starts games in their emulator.
 */
#pragma once

#include "core/Game.h"
#include "core/Settings.h"
#include "core/LaunchResult.h"
#include "core/EmulatorTemplate.h"
#include "library/EmulatorManager.h"

/**
 * @brief Starts a selected game using the matching emulator settings.
 */
class GameLauncher {

public:
    /**
     * @brief Creates a launcher that can ask the emulator manager for settings.
     *
     * @param emulatorManager Manager used to find emulator configuration.
     */
    explicit GameLauncher(EmulatorManager& emulatorManager);

    /**
     * @brief Checks the ROM and emulator setup, then starts the game.
     *
     * @param game Game record the user wants to launch.
     * @param settings Current app settings with emulator templates.
     * @return Launch result with success flag, message, and process pointer.
     */
    LaunchResult launch(const Game& game, const Settings& settings);

private:
    EmulatorManager& emulatorManager;
};

/**
 * @file UIController.h
 * @brief Declares the UI controller that mediates between windows and services.
 */
#pragma once

#include "ui/MainWindow.h"
#include "ui/SettingsWindow.h"
#include "library/LibraryService.h"
#include "settings/SettingsManager.h"
#include "library/EmulatorManager.h"
#include "ui/ControllerService.h"


#include <string>
#include <QTimer>
#include <QMessageBox>

/**
 * @brief Coordinates UI events between the windows, services, and background scan timer.
 */
class UIController {

public:
    /**
     * @brief Creates the UI controller and wires up the application windows.
     *
     * @param libraryService Service used for library scanning and game actions.
     * @param settingsManager Manager used to load the current settings.
     * @param emulatorManager Manager used for emulator setup and installs.
     */
    UIController(LibraryService& libraryService, SettingsManager& settingsManager, EmulatorManager& emulatorManager);

    /**
     * @brief Stops controller input before the UI controller is destroyed.
     */
    ~UIController();

    /**
     * @brief Shows the library screen and ensures periodic scanning is active.
     */
    void showLibrary();

    /**
     * @brief Loads and displays details for a selected game.
     *
     * @param gameID Identifier of the selected game.
     */
    void showDetails(const std::string& gameID);

    /**
     * @brief Shows the settings screen and pauses background scanning.
     */
    void showSettings();

    /**
     * @brief Displays an application error message to the user.
     *
     * @param message Human-readable error text.
     */
    void showError(const std::string& message);

    /**
     * @brief Returns the main application window.
     *
     * @return Reference to the main window instance.
     */
    MainWindow& getWindow();

    /**
     * @brief Runs the first-time emulator setup dialog if needed.
     */
    void runInitialSetup();

private: 
    /**
     * @brief Runs a guarded library scan and refreshes the visible game list.
     */
    void runBackgroundScan();

    /**
     * @brief Rebuilds the visible game list using the active filters and sort mode.
     */
    void refreshLibraryView();

    /**
     * @brief Toggles the favourite state of the currently selected game.
     */
    void toggleFavourite();

    /**
     * @brief Launches the currently selected game.
     */
    void launchSelectedGame();

    /**
     * @brief Installs emulator requests chosen in the settings window.
     *
     * @param requests Requested systems and install folders.
     */
    void installGivenEmulators(const std::vector<InstallRequest>& requests);

    /**
     * @brief Shows a confirmation box before exiting the app.
     */
    void promptExit();

    /**
     * @brief Lets controller buttons accept or close an active modal dialog.
     *
     * @param actions Controller actions from the latest poll.
     * @return `true` when a modal dialog was active.
     */
    bool handleActiveModalDialog(const Actions& actions);

    MainWindow window;
    SettingsWindow settingsWindow;
    LibraryService& libraryService;
    SettingsManager& settingsManager;
    EmulatorManager& emulatorManager;

    ControllerService controllerInput;

    QTimer scanTimer;
    QTimer controllerPollTimer;
    bool scanFlag = false;
    QMessageBox* promptBox = nullptr;
    bool gameRunning = false;
};

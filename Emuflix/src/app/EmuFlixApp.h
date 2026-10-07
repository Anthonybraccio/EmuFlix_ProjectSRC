/**
 * @file EmuFlixApp.h
 * @brief Declares the application composition root.
 */
#pragma once

#include "data/SQLiteLibrary.h"
#include "data/SQLiteSettings.h"
#include "library/LibraryService.h"
#include "settings/SettingsManager.h"
#include "ui/UIController.h"

#include "data/SQLiteDatabase.h"

/**
 * @brief Wires together the application's persistence, service, and UI layers.
 */
class EmuFlixApp {

public:
    /**
     * @brief Builds the application object graph.
     */
    EmuFlixApp();

    /**
     * @brief Opens the database, initializes the schema, and shows the main UI.
     */
    void start();

    /**
     * @brief Performs application shutdown work before the process exits.
     */
    void shutdown();

private:
    SQLiteDatabase database;

    SQLiteLibrary library;
    EmulatorManager emulatorManager;
    SQLiteSettings settingsStore;

    SettingsManager settingsManager;
    LibraryService libraryService;

    UIController uiController;
};

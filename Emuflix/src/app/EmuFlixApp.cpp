/**
 * @file EmuFlixApp.cpp
 * @brief Builds and coordinates the application's core components.
 */

#include "app/EmuFlixApp.h"
#include "data/DatabasePath.h"

/**
 * @brief Constructs the application object graph.
 */
EmuFlixApp::EmuFlixApp() :
    database(),
    library(database),
    emulatorManager(),
    settingsStore(database, emulatorManager),
    settingsManager(settingsStore),
    libraryService(library, emulatorManager),
    uiController(libraryService, settingsManager, emulatorManager) {
}


/**
 * @brief Opens the database, initializes the schema, and shows the main UI.
 */
void EmuFlixApp::start() {
    QString error;
    const QString databasePath = DatabasePath::getDatabaseFilePath();

    if(database.open(databasePath, &error) == false) {
        uiController.showError(("Failed to open database: " + error).toStdString());
        return;
    }

    if(database.initializeSchema(&error) == false) {
        uiController.showError(("Failed to initialize database schema: " + error).toStdString());
        return;
    }

    uiController.runInitialSetup();
    uiController.showLibrary();
    uiController.getWindow().show();
}


/**
 * @brief Shutdown application.
 */
void EmuFlixApp::shutdown() {
    //nothing needed as the deconstuctors are in classes.
}

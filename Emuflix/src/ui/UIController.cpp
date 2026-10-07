/**
 * @file UIController.cpp
 * @brief Implements coordination between the UI and application services.
 */

#include "ui/UIController.h"
#include "ui/SetupWindow.h"

#include <QStringList>
#include <QString>
#include <QObject>
#include <QApplication>
#include <QDialog>
#include <QCoreApplication>
#include <QKeyEvent>

/**
 * @brief Wires up UI windows, connects signals, and schedules background scans.
 *
 * @param libraryService Service used for library operations.
 * @param settingsManager Manager used to load and save settings.
 * @param emulatorManager Manager used for emulator setup and installs.
 */
UIController::UIController(LibraryService& libraryService, SettingsManager& settingsManager, EmulatorManager& emulatorManager) 
    : window(), settingsWindow(settingsManager, emulatorManager), libraryService(libraryService), settingsManager(settingsManager), emulatorManager(emulatorManager) {
    window.setSettingsWindow(&settingsWindow);
    
    QObject::connect(&window, &MainWindow::settingsClicked, [&]() {
        showSettings();
    });

    QObject::connect(&window, &MainWindow::favouriteClicked, [&]() {
        toggleFavourite();
    });

    QObject::connect(&window, &MainWindow::LaunchClicked, [&]() {
        launchSelectedGame();
    });

    QObject::connect(&window, &MainWindow::gameSelectionChanged, [&](const std::string& gameID) {
        showDetails(gameID);
    });

    QObject::connect(&window, &MainWindow::searchChanged, [&]() {
        refreshLibraryView();
    });

    QObject::connect(&window, &MainWindow::favouriteOnlyChanged, [&]() {
        refreshLibraryView();
    });

    QObject::connect(&window, &MainWindow::sortChanged, [&]() {
        refreshLibraryView();
    });

    QObject::connect(&settingsWindow, &SettingsWindow::backClicked, [&]() {
        showLibrary();
    });

    QObject::connect(&settingsWindow, &SettingsWindow::installEmulatorsClicked, [&](const std::vector<InstallRequest>& requests) {
        installGivenEmulators(requests);
    });

    QObject::connect(&libraryService, &LibraryService::gameLaunchFinished, &window, [&]() {
        gameRunning = false;
    });

    QObject::connect(&libraryService, &LibraryService::gameLaunchFailed, &window, [&](const QString& message) {
        gameRunning = false;

        window.showError(message);
    });

    scanTimer.setInterval(10000);
    QObject::connect(&scanTimer, &QTimer::timeout, [&]() {
        runBackgroundScan();
    });

    controllerInput.initialize();
    controllerPollTimer.setInterval(16);

    QObject::connect(&controllerPollTimer, &QTimer::timeout, [&]() {
        Actions actions = controllerInput.poll();

        if(gameRunning == true) {
            return;
        }

        if(window.isLibraryVisible() == false) {
            return;
        }

        if(handleActiveModalDialog(actions) == true) {
            return;
        }

        if(actions.moveLeft == true) {
            window.moveFocusLeft();
        }

        if(actions.moveRight == true) {
            window.moveFocusRight();
        }

        if(actions.moveUp == true) {
            window.moveFocusUp();
        }

        if(actions.moveDown == true) {
            window.moveFocusDown();
        }

        if(actions.sortLeft == true) {
            window.cycleSortLeft();
        }

        if(actions.sortRight == true) {
            window.cycleSortRight();
        }

        if(actions.toggleFav == true) {
            toggleFavourite();
        }

        if(actions.toggleFavOnly == true) {
            window.toggleFavouriteFilter();
        }

        if(actions.openSettings == true) {
            showSettings();
        }

        if(actions.launch == true) {
            launchSelectedGame();
        }

        if(actions.requestExit == true) {
            promptExit();
        }
    });

    controllerPollTimer.start();
}

/**
 * @brief Stops controller polling and shuts down controller input.
 */
UIController::~UIController() {
    controllerPollTimer.stop();
    controllerInput.shutdown();
}

/**
 * @brief Displays the library view and ensures scans are running.
 */
void UIController::showLibrary() {
    window.showLibrary();
    runBackgroundScan();
    
    if(scanTimer.isActive() == false) {
        scanTimer.start();
    }
}


/**
 * @brief Loads and displays details for the selected game.
 *
 * @param gameID Identifier of the selected game.
 */
void UIController::showDetails(const std::string& gameID) {
    Game game = libraryService.getGameDetails(gameID);
    if(game.id.empty() == true) {
        showError("Game not found");
        return;
    }

    std::string favText;
    if(game.favourite == true) {
        favText = "Yes";
    }
    else {
        favText = "No";
    }

    std::string lastPlayedDate;
    if(game.lastPlayed.empty() == true) {
        lastPlayedDate = "Never";
    }
    else {
        lastPlayedDate = game.lastPlayed;
    }

    std::string systemText;
    if(game.system.empty() == true) {
        systemText = "Unknown";
    }
    else {
        systemText = game.system;
    }

    std::string timePlayedText;
    if (game.timePlayed == 0) {
        timePlayedText = "Never played";
    } else {
        int h = game.timePlayed / 3600;
        int m = (game.timePlayed % 3600) / 60;
        if (h > 0) timePlayedText += std::to_string(h) + "h ";
        timePlayedText += std::to_string(m) + "m";
    }

    QString details = QString("Title: %1\nSystem: %2\nROM Path: %3\nFavourited: %4\nLast Played: %5\nTime Played: %6")
        .arg(QString::fromStdString(game.title))
        .arg(QString::fromStdString(systemText))
        .arg(QString::fromStdString(game.romPath))
        .arg(QString::fromStdString(favText))
        .arg(QString::fromStdString(lastPlayedDate))
        .arg(QString::fromStdString(timePlayedText));
    window.setDetailsText(details);
    window.setFavouriteButtonText(game.favourite);
}


/**
 * @brief Shows the settings window and pauses background scanning.
 */
void UIController::showSettings() {
    if(scanTimer.isActive() == true) {
        scanTimer.stop();
    }

    settingsWindow.loadSettings();
    window.showSettingsWindow();
}


/**
 * @brief Displays an error message via the main window.
 *
 * @param message Human-readable text to show.
 */
void UIController::showError(const std::string& message) {
    QString error = QString("Error: %1")
        .arg(QString::fromStdString(message));

    window.showError(error);
}


/**
 * @brief Returns the main application window.
 *
 * @return Reference to the main window instance.
 */
MainWindow& UIController::getWindow() {
    return window;
}


/**
 * @brief Shows first-time setup and installs selected emulators if requested.
 */
void UIController::runInitialSetup() {
    Settings settings = settingsManager.load();
    if(settings.setupCompleted == true) {
        return;
    }

    SetupWindow setupWindow(emulatorManager.getSupportedSystems(), &window);

    int dialogResult = setupWindow.exec();
    if(dialogResult == QDialog::Accepted) {
        std::vector<std::string> systems = setupWindow.getSelectedSystems();
        emulatorManager.applySetupSelections(settings, systems, true);

        std::vector<SetupInstallSelection> selections = setupWindow.getInstallSelections();
        if(selections.empty() == true) {
            settingsManager.save(settings);
        }
        else {
            std::vector<InstallRequest> installRequests;
            QStringList installedSystems;
            QStringList failedSystems;

            for(const SetupInstallSelection& selection : selections) {
                InstallRequest request;

                request.system = selection.system;
                request.destination = selection.folder;
                installRequests.push_back(request);
            }

            std::vector<InstallResult> installResults = emulatorManager.installRequestedSystems(settings, installRequests);
            settingsManager.save(settings);

            for(const InstallResult& result : installResults) {
                if(result.sucess == true) {
                    installedSystems.push_back(QString::fromStdString(result.system));
                }
                else {
                    QString failureLine = QString("%1 - %2")
                        .arg(QString::fromStdString(result.system))
                        .arg(QString::fromStdString(result.message));

                    failedSystems.push_back(failureLine);
                }
            }

            QString summaryMessage;
            if(installedSystems.empty() == false) {
                summaryMessage += "Installed successfully:\n";

                for(const QString& installedSystem : installedSystems) {
                    summaryMessage += " -> " + installedSystem + "\n";
                }
            }

            if(failedSystems.empty() == false) {
                if(summaryMessage.isEmpty() == false) {
                    summaryMessage += "\n";
                }
                summaryMessage += "Failed installs:\n";

                for(const QString& failedSystem : failedSystems) {
                    summaryMessage += " -> " + failedSystem + "\n";
                }
            }

            if(summaryMessage.isEmpty() == true) {
                summaryMessage = "No emulator installed were processed.";
            }
            QMessageBox::information(&window, "Emulator Installation", summaryMessage);
        }
    }

    
    settingsManager.save(settings);
    settings = settingsManager.load();
    
    if(emulatorManager.getInstallSystems(settings).empty() == true) {
        QMessageBox::warning(
            &window,
            "No Emulators Selected",
            "No emulators were selected. Games will need emulators installed before they can launch."
        );
    }
}


/**
 * @brief Runs a guarded library scan and refreshes the visible library.
 */
void UIController::runBackgroundScan() {
    if(scanFlag == true) {
        return;
    }

    scanFlag = true;

    Settings settings = settingsManager.load();
    libraryService.scanLibrary(settings);
    refreshLibraryView();

    scanFlag = false;
}


/**
 * @brief Rebuilds the library list based on current filters and sort order.
 */
void UIController::refreshLibraryView() {
    std::vector<Game> games = libraryService.getFilteredLibrary(
        window.getSearchText(),
        window.favouriteOnlyEnabled(),
        window.getSelectedSortOption()
    );

    window.setGames(games);
}


/**
 * @brief Toggles the favourite state of the currently selected game.
 */
void UIController::toggleFavourite() {
    std::string gameID = window.getSelectedGameId();

    if(gameID.empty() == true) {
        showError("No game selected to favourite");
        return;
    }

    libraryService.toggleFavourite(gameID);
    refreshLibraryView();
}


/**
 * @brief Launches the selected game and marks the app as running a game.
 */
void UIController::launchSelectedGame() {
    std::string gameID = window.getSelectedGameId();

    if(gameID.empty() == true) {
        showError("No game currently selected");
        return;
    }

    Settings settings = settingsManager.load();
    LaunchResult result = libraryService.launchGame(gameID, settings);

    if(result.success == false) {
        showError(result.message);
        return;
    }

    gameRunning = true;
    showDetails(gameID);
}


/**
 * @brief Installs emulator packages requested from the settings screen.
 *
 * @param requests Systems and install folders selected by the user.
 */
void UIController::installGivenEmulators(const std::vector<InstallRequest>& requests) {
    Settings settings = settingsManager.load();
    QStringList installedSystems;
    QStringList failedSystems;

    std::vector<InstallResult> installResults = emulatorManager.installRequestedSystems(settings, requests);
    settingsManager.save(settings);
    settingsWindow.loadSettings();
    
    for(const InstallResult& result : installResults) {
        if(result.sucess == true) {
            installedSystems.push_back(QString::fromStdString(result.system));
        }
        else {
            QString failureLine = QString("%1 - %2")
            .arg(QString::fromStdString(result.system))
            .arg(QString::fromStdString(result.message));

            failedSystems.push_back(failureLine);
        }
    }

    QString summaryMessage;
    if(installedSystems.empty() == false) {
        summaryMessage += "Installed successfully:\n";

        for(const QString& installedSystem : installedSystems) {
            summaryMessage += " -> " + installedSystem + "\n";
        }
    }

    if(failedSystems.empty() == false) {
        if(summaryMessage.isEmpty() == false) {
            summaryMessage += "\n";
        }
        summaryMessage += "Failed installs:\n";

        for(const QString& failedSystem : failedSystems) {
            summaryMessage += " -> " + failedSystem + "\n";
        }
    }

    if(summaryMessage.isEmpty() == true) {
        summaryMessage = "No emulator installed were processed.";
    }
    QMessageBox::information(&window, "Emulator Installation", summaryMessage);
}


/**
 * @brief Opens an exit confirmation dialog.
 */
void UIController::promptExit() {
    if(promptBox != nullptr) {
        return;
    }

    promptBox = new QMessageBox(
        QMessageBox::Question,
        "Exit EmuFlix",
        "Do you want to exit EmuFlix?",
        QMessageBox::Yes | QMessageBox::No,
        &window
    );

    promptBox->setDefaultButton(QMessageBox::Yes);
    promptBox->setEscapeButton(QMessageBox::No);
    promptBox->setAttribute(Qt::WA_DeleteOnClose, true);

    QObject::connect(promptBox, &QMessageBox::finished, &window, [this](int result) {
        promptBox = nullptr;

        if(result == QMessageBox::Yes) {
            QApplication::quit();
        }
    });

    promptBox->open();
}


/**
 * @brief Sends controller confirm or cancel input to an active modal dialog.
 *
 * @param actions Controller actions from the current frame.
 * @return `true` when a modal dialog was active and handled.
 */
bool UIController::handleActiveModalDialog(const Actions& actions) {
    QWidget* activeModal = QApplication::activeModalWidget();
    
    if(activeModal == nullptr) {
        return false;
    }
    
    if(actions.launch == true) {
        QKeyEvent pressEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QKeyEvent releaseEvent(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier);

        QCoreApplication::sendEvent(activeModal, &pressEvent);
        QCoreApplication::sendEvent(activeModal, &releaseEvent);

        return true;
    }

    if(actions.requestExit == true) {
        QKeyEvent pressEvent(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QKeyEvent releaseEvent(QEvent::KeyRelease, Qt::Key_Escape, Qt::NoModifier);

        QCoreApplication::sendEvent(activeModal, &pressEvent);
        QCoreApplication::sendEvent(activeModal, &releaseEvent);

        return true;
    }

    return true;
}

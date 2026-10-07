/**
 * @file main.cpp
 * @brief Application entry point for EmuFlix.
 */

#include "app/EmuFlixApp.h"

#include <QApplication>
#include <QCoreApplication>
#include <QObject>
#include <iostream>

/**
 * @brief Creates the Qt application and starts the EmuFlix event loop.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument values.
 * @return Exit code reported by the Qt event loop or an early startup failure.
 */
int main(int argc, char* argv[]) {
    QApplication qtScreen(argc, argv);

    if(QCoreApplication::instance() == nullptr) {
        std::cerr << "Failed to create a Qt application.\n";
        return 1;
    }

    EmuFlixApp app;

    QObject::connect(&qtScreen, &QApplication::aboutToQuit, [&]() {
        app.shutdown();
    });

    app.start();
    return qtScreen.exec();
}

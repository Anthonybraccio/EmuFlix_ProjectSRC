/**
 * @file DatabasePath.cpp
 * @brief Resolves and prepares the database location on disk.
 */

#include "data/DatabasePath.h"

#include <QDir>
#include <QStandardPaths>

/**
 * @brief Returns the absolute path to the SQLite database file.
 *
 * Creates the `Emuflix` application data folder when necessary.
 *
 * @return Fully qualified database file path.
 */
QString DatabasePath::getDatabaseFilePath() {
    QString DBDirectory = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir dir(DBDirectory);


    if(dir.exists() ==  false) {
        dir.mkpath(".");
    }

    if(dir.exists("Emuflix") == false) {
        dir.mkdir("Emuflix");
    }

    dir.cd("Emuflix");
    return dir.filePath("emuflix.sqlite");
}

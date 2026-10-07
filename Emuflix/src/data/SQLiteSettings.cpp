/**
 * @file SQLiteSettings.cpp
 * @brief Implements the SQLite-backed settings repository.
 */

#include "data/SQLiteSettings.h"

#include <QtSql/QSqlQuery>
#include <QVariant>
#include <QString>
#include <unordered_set>

static void checkTemplateExists(Settings& settings, const std::string& system) {
    for(const EmulatorTemplate& emulatorTemplate : settings.emulatorTemplates) {
        if(emulatorTemplate.system == system) {
            return;
        }
    }

    EmulatorTemplate emulatorTemplate;
    emulatorTemplate.system = system;
    emulatorTemplate.emulatorPath = "";
    emulatorTemplate.arguments = "";
    emulatorTemplate.enabled = true;

    settings.emulatorTemplates.push_back(emulatorTemplate);
}


/**
 * @brief Constructs the settings repository wrapper.
 *
 * @param db Database wrapper that provides the active SQLite connection.
 */
SQLiteSettings::SQLiteSettings(SQLiteDatabase& db, EmulatorManager& emulatorManager)
    : db(db), emulatorManager(emulatorManager) {
}


/**
 * @brief Loads the persisted settings from the database.
 *
 * @return Settings snapshot populated from the database, or defaults when unavailable.
 */
Settings SQLiteSettings::load() {
    Settings settings;

    QSqlDatabase& database = db.getDatabase();
    if(database.isOpen() == false) {
        return settings;
    }

    QSqlQuery romQuery(database);
    romQuery.prepare(
        "SELECT path "
        "FROM rom_folders "
        "WHERE enabled = 1 "
        "ORDER BY addedAt ASC, path ASC"
    );

    if(romQuery.exec() == true) {
        while(romQuery.next() == true) {
            const QString romFilePath = romQuery.value(0).toString();

            if(romFilePath.isEmpty() == false) {
                settings.romDirectories.push_back(romFilePath.toStdString());
            }
        }
    }

    QSqlQuery templateQuery(database);
    templateQuery.prepare(
        "SELECT systemType, emulatorPath, arguments, enabled "
        "FROM emulator_paths "
        "ORDER BY id ASC"
    );

    std::unordered_set<std::string> loadedSystems;

    if(templateQuery.exec() == true) {
        while(templateQuery.next() == true) {
            EmulatorTemplate emulator;

            emulator.system = templateQuery.value("systemType").toString().toStdString();
            emulator.emulatorPath = templateQuery.value("emulatorPath").toString().toStdString();
            emulator.arguments = templateQuery.value("arguments").toString().toStdString();
            emulator.enabled = templateQuery.value("enabled").toInt() != 0;

            if(emulator.arguments.empty() == true) {
                emulator.arguments = "";
            }

            if(loadedSystems.contains(emulator.system) == true) continue;

            settings.emulatorTemplates.push_back(emulator);
            loadedSystems.insert(emulator.system);
        }
    }

    for(const std::string& system : emulatorManager.getSupportedSystems()) {
        checkTemplateExists(settings, system);
    }

    QSqlQuery setupFlagQuery(database);
    setupFlagQuery.prepare(
        "SELECT value "
        "FROM setup_flag "
        "WHERE key = :key"
    );
    setupFlagQuery.bindValue(":key", "setupCompleted");

    if(setupFlagQuery.exec() == true && setupFlagQuery.next() == true) {
        settings.setupCompleted = setupFlagQuery.value(0).toString() == "1";
    }

    QSqlQuery selectedSystemsQuery(database);
    selectedSystemsQuery.prepare(
        "SELECT systemName "
        "FROM setup_systems "
        "ORDER BY systemName ASC"
    );

    if(selectedSystemsQuery.exec() == true) {
        while(selectedSystemsQuery.next() == true) {
            const QString systemName = selectedSystemsQuery.value(0).toString();

            if(systemName.isEmpty() == false) {
                settings.selectedInstallSystems.push_back(systemName.toStdString());
            }
        }
    }



    return settings;
}

/**
 * @brief Saves the provided settings snapshot to the database.
 *
 * @param settings Values to persist.
 */
void SQLiteSettings::save(const Settings& settings) {
    QSqlDatabase& database = db.getDatabase();

    if(database.isOpen() == false) return;
    if(database.transaction() == false) return;

    QSqlQuery removeROMFolders(database);
    if(removeROMFolders.exec("DELETE FROM rom_folders") == false) {
        database.rollback();
        return;
    }

    QSqlQuery insertROMFolders(database);
    insertROMFolders.prepare(
        "INSERT OR REPLACE INTO rom_folders (path, enabled) "
        "VALUES (:path, 1) "
    );

    std::unordered_set<std::string> checkedPaths;

    for(const std::string& folder :settings.romDirectories) {
        if(folder.empty() == true) continue;
        if(checkedPaths.find(folder) != checkedPaths.end()) continue;

        checkedPaths.insert(folder);
        insertROMFolders.bindValue(":path", QString::fromStdString(folder));

        if(insertROMFolders.exec() == false) {
            database.rollback();
            return;
        }
    }

    QSqlQuery deleteEmulatorPaths(database);
    if(deleteEmulatorPaths.exec("DELETE FROM emulator_paths") == false) {
        database.rollback();
        return;
    }

    std::unordered_set<std::string> savedSystems;

    for(const EmulatorTemplate& emulatorTemplate : settings.emulatorTemplates) {
        if(emulatorTemplate.system.empty() == true) continue;
        if(savedSystems.contains(emulatorTemplate.system) == true) continue;

        savedSystems.insert(emulatorTemplate.system);

        QString path = QString::fromStdString(emulatorTemplate.emulatorPath).trimmed();
        QString args = QString::fromStdString(emulatorTemplate.arguments).trimmed();

        QSqlQuery insertEmulatorTemplates(database);
        insertEmulatorTemplates.prepare(
            "INSERT INTO emulator_paths (systemType, emulatorPath, arguments, enabled) "
            "VALUES (:systemType, :emulatorPath, :arguments, :enabled)"
        );

        insertEmulatorTemplates.bindValue(":systemType", QString::fromStdString(emulatorTemplate.system));
        insertEmulatorTemplates.bindValue(":emulatorPath", path);
        insertEmulatorTemplates.bindValue(":arguments", args);
        insertEmulatorTemplates.bindValue(":enabled", emulatorTemplate.enabled ? 1 : 0);

        if(insertEmulatorTemplates.exec() == false) {
            database.rollback();
            return;
        }
    }

    QSqlQuery deleteSetupFlag(database);
    if(deleteSetupFlag.exec("DELETE FROM setup_flag") == false) {
        database.rollback();
        return;
    }

    QSqlQuery insertSetupFlag(database);
    insertSetupFlag.prepare(
        "INSERT INTO setup_flag (key, value) "
        "VALUES (:key, :value)"
    );

    insertSetupFlag.bindValue(":key", "setupCompleted");
    insertSetupFlag.bindValue(":value", settings.setupCompleted ? "1" : "0");

    if(insertSetupFlag.exec() == false) {
        database.rollback();
        return;
    }

    QSqlQuery deleteSetupSystems(database);
    if(deleteSetupSystems.exec("DELETE FROM setup_systems") == false) {
        database.rollback();
        return;
    }

    std::unordered_set<std::string> savedSelectedSystems;

    QSqlQuery insertSelectedSystem(database);
    insertSelectedSystem.prepare(
        "INSERT INTO setup_systems (systemName) "
        "VALUES (:systemName)"
    );

    for(const std::string& system : settings.selectedInstallSystems) {
        if(system.empty() == true) continue;
        if(savedSelectedSystems.contains(system) == true) continue;

        savedSelectedSystems.insert(system);
        insertSelectedSystem.bindValue(":systemName", QString::fromStdString(system));

        if(insertSelectedSystem.exec() == false) {
            database.rollback();
            return;
        }
    }

    if(database.commit() == false) {
        database.rollback();
        return;
    }
}

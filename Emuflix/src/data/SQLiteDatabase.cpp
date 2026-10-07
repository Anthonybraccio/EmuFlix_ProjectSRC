/**
 * @file SQLiteDatabase.cpp
 * @brief Implements the SQLite connection wrapper and schema management helpers.
 */

#include "data/SQLiteDatabase.h"

#include <QFile>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTextStream>
#include <QUuid>
#include <QCoreApplication>

/**
 * @brief Loads the schema SQL file from disk.
 *
 * @param filePath Absolute path to the schema file.
 * @param errOutput Optional destination for a human-readable error message.
 * @return Full text contents of the file, or an empty string on failure.
 */
static QString readSqlFile(const QString filePath, QString* errOutput) {
    QString fileContent;
    QFile file(filePath);

    if(file.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        if(errOutput != nullptr) {
            *errOutput = QString("Failed to open Schema SQL file: %1")
                .arg(file.errorString());
        }

        return {};
    }

    QTextStream in(&file);
    fileContent = in.readAll();
    return fileContent;
}


/**
 * @brief Splits a schema file into executable SQL statements.
 *
 * @param sqlContent Raw schema text loaded from disk.
 * @return List of semicolon-terminated SQL statements.
 */
static QStringList splitSqlStatements(const QString& sqlContent) {
    QStringList output;
    QString currentStatement;

    for(QChar c : sqlContent) {
        currentStatement += c;

        if(c == ';') {
            const QString trimmed = currentStatement.trimmed();

            if(trimmed.isEmpty() == false) {
                output.append(trimmed);
            }

            currentStatement.clear();
        }
    }

    const QString endString = currentStatement.trimmed();

    if(endString.isEmpty() == false) {
        output.push_back(endString);
    }

    return output;
}


/**
 * @brief Initializes the database wrapper with a unique connection name.
 */
SQLiteDatabase::SQLiteDatabase() {
    uniqueName = QString("emuflix_sqlite_%1")
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}


/**
 * @brief Closes the connection and deregisters it from Qt's database pool.
 */
SQLiteDatabase::~SQLiteDatabase() {
    if(db.isValid() == true) {
        db.close();
    }

    db = QSqlDatabase();
    QSqlDatabase::removeDatabase(uniqueName);
}


/**
 * @brief Opens or creates the SQLite database at the provided path.
 *
 * @param filePath Target database file path.
 * @param errOutput Optional error text when opening fails.
 * @return `true` on success, otherwise `false`.
 */
bool SQLiteDatabase::open(const QString& filePath, QString* errOutput) {
    if(QSqlDatabase::contains(uniqueName)) {
        db = QSqlDatabase::database(uniqueName);
    }
    else {
        db = QSqlDatabase::addDatabase("QSQLITE", uniqueName);
    }

    db.setDatabaseName(filePath);

    if(db.open() == false) {
        if(errOutput != nullptr) {
            *errOutput = db.lastError().text();
        }
        return false;
    }

    return true;
}


/**
 * @brief Applies the bundled schema SQL to the open database.
 *
 * @param errOutput Optional destination for any error encountered.
 * @return `true` if every statement executed successfully.
 */
bool SQLiteDatabase::initializeSchema(QString* errOutput) {
    if(db.isValid() == false || db.isOpen() == false) {
        if(errOutput != nullptr) {
            *errOutput = "Database is not open";
        }

        return false;
    }

    const QString schemaPath = QCoreApplication::applicationDirPath() + "/schema.sql";
    
    QString readError;
    const QString sqlContent = readSqlFile(schemaPath, &readError);

    if(sqlContent.isEmpty() == true) {
        if(errOutput != nullptr) {
            *errOutput = readError;
        }

        return false;
    }

    QSqlQuery query(db);
    const QStringList statements = splitSqlStatements(sqlContent);

    for(const QString& statement: statements) {
        const QString trimmed = statement.trimmed();

        if(trimmed.isEmpty() == true) {
            continue;
        }

        if(query.exec(trimmed) == false) {
            if(errOutput != nullptr) {
                *errOutput = QString("Schema execution failed: %1\nStatement: %2")
                    .arg(query.lastError().text())
                    .arg(trimmed);
            }

            return false;
        }
    }

    return true;

}


/**
 * @brief Provides access to the managed `QSqlDatabase` instance.
 *
 * @return Reference to the active database connection.
 */
QSqlDatabase& SQLiteDatabase::getDatabase() {
    return db;
}

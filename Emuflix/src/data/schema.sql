/*!
 * @file schema.sql
 * @brief Defines the SQLite schema for EmuFlix.
 */

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS rom_folders (
    path        TEXT PRIMARY KEY,
    enabled     INTEGER NOT NULL DEFAULT 1,
    addedAt     TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS games (
    id          TEXT PRIMARY KEY,
    name        TEXT NOT NULL,
    romPath     TEXT NOT NULL UNIQUE,
    addedAt     TEXT NOT NULL DEFAULT (datetime('now')),
    lastSeen    TEXT,
    lastPlayed  TEXT,
    timePlayed  INTEGER NOT NULL DEFAULT 0,
    system      TEXT
);

CREATE TABLE IF NOT EXISTS game_settings (
    gameID      TEXT PRIMARY KEY,
    favourite   INTEGER NOT NULL DEFAULT 0,
    FOREIGN KEY(gameID) REFERENCES games(id) ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS emulator_paths (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    systemType      TEXT NOT NULL,
    emulatorPath    TEXT NOT NULL,
    arguments       TEXT NOT NULL DEFAULT '',
    enabled         INTEGER NOT NULL DEFAULT 1,
    addedAt         TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS setup_flag (
    key         TEXT PRIMARY KEY,
    value       TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS setup_systems (
    systemName      TEXT PRIMARY KEY
);

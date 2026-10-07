/**
 * @file Game.h
 * @brief Declares the Game value object.
 */
#pragma once
#include <string>

/**
 * @brief Represents a game discovered in the ROM library.
 */
struct Game {
    std::string id;        ///< Stable identifier used by the application and database.
    std::string title;     ///< Display title derived from the ROM file name.
    std::string romPath;   ///< Absolute path to the ROM file on disk.
    bool favourite = false; ///< Indicates whether the game is marked as a favourite.
    std::string lastPlayed;
    int timePlayed = 0;
    std::string system;
};

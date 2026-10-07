/**
 * @file EmulatorInstaller.h
 * @brief Declares the helper that installs bundled emulator packages.
 */
#pragma once

#include <string>

class QString;

/**
 * @brief Result from trying to install one emulator.
 */
struct InstallResult {
    std::string system;         ///< System the install was for.
    bool sucess = false;        ///< True when the install completed successfully.
    std::string executablePath; ///< Path to the installed emulator exe.
    std::string message;        ///< Message explaining what happened.
};

/**
 * @brief Installs emulator zip files that are bundled with the app.
 */
class EmulatorInstaller {

public:
    /**
     * @brief Installs the emulator for a selected system.
     *
     * @param system Console system to install an emulator for.
     * @param destination Folder where the emulator should be extracted.
     * @return Install result with success, message, and exe path.
     */
    InstallResult installSystem(const std::string& system, const std::string& destination) const;

private:
    /**
     * @brief Gets the folder that contains bundled emulator packages.
     *
     * @return Assets folder path.
     */
    QString getAssetsPath() const;

    /**
     * @brief Gets the zip package path for a system.
     *
     * @param system System name to install.
     * @return Package path, or empty string if unsupported.
     */
    QString getPackagePath(const std::string& system) const;

    /**
     * @brief Gets the expected exe path after extraction.
     *
     * @param system System name to check.
     * @return Relative exe path inside the install folder.
     */
    QString getInstalledSystemPath(const std::string& system) const;

    /**
     * @brief Checks if the system uses the Mesen emulator package.
     *
     * @param system System name to check.
     * @return `true` for Mesen systems.
     */
    bool isMesen(const std::string& system) const;

    /**
     * @brief Checks if the system uses the gopher64 emulator package.
     *
     * @param system System name to check.
     * @return `true` for Nintendo 64.
     */
    bool isGopher(const std::string& system) const;

    /**
     * @brief Checks that the install destination exists or can be created.
     *
     * @param destination Folder chosen by the user.
     * @param errorMessage Output message when the folder is not usable.
     * @return `true` when the destination is usable.
     */
    bool checkDestination(const QString& destination, QString& errorMessage) const;

    /**
     * @brief Extracts an emulator zip into the destination folder.
     *
     * @param zipPath Path to the emulator package zip.
     * @param destination Folder where files should be extracted.
     * @param errorMessage Output message when extraction fails.
     * @return `true` when extraction succeeds.
     */
    bool extractZip(const QString& zipPath, const QString& destination, QString& errorMessage) const;
};

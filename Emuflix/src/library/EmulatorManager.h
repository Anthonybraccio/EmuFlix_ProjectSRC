/**
 * @file EmulatorManager.h
 * @brief Declares helper logic for emulator setup and status.
 */
#pragma once

#include "core/EmulatorStatus.h"
#include "core/Settings.h"
#include "library/EmulatorInstaller.h"

#include <string>
#include <vector>

/**
 * @brief Request to install an emulator for one system.
 */
struct InstallRequest {
    std::string system;      ///< Console system being installed.
    std::string destination; ///< Folder where the emulator should be installed.
};

/**
 * @brief Manages supported systems and emulator configuration.
 */
class EmulatorManager {

public:
    /**
     * @brief Creates an emulator manager with the default supported systems.
     */
    EmulatorManager() = default;

    /**
     * @brief Returns every system the app knows how to handle.
     *
     * @return List of supported system names.
     */
    const std::vector<std::string>& getSupportedSystems() const;
    
    /**
     * @brief Finds the emulator template for a system in the settings.
     *
     * @param settings Current app settings.
     * @param system System name to look up.
     * @return Matching template, or `nullptr` when none exists.
     */
    const EmulatorTemplate* findTemplate(const Settings& settings, const std::string& system) const;

    /**
     * @brief Checks if an emulator template points to a usable emulator.
     *
     * @param emulatorTemplate Template to check.
     * @return Current status for the emulator.
     */
    EmulatorStatus getStatus(const EmulatorTemplate* emulatorTemplate) const;

    /**
     * @brief Gets the systems selected during first-time setup.
     *
     * @param settings Current app settings.
     * @return Selected systems list.
     */
    const std::vector<std::string>& getInstallSystems(const Settings& settings) const;

    /**
     * @brief Checks if the first setup flow has been completed.
     *
     * @param settings Current app settings.
     * @return `true` when setup is marked complete.
     */
    bool isSetupCompleted(const Settings& settings) const;

    /**
     * @brief Checks whether one system was selected for install.
     *
     * @param settings Current app settings.
     * @param system System name to check.
     * @return `true` when the system is selected.
     */
    bool isSystemInstallSelected(const Settings& settings, const std::string& system) const;

    /**
     * @brief Stores setup choices back into the settings object.
     *
     * @param settings Settings object to update.
     * @param selectedSystems Systems chosen by the user.
     * @param setupCompleted Whether setup should be marked finished.
     */
    void applySetupSelections(Settings& settings, const std::vector<std::string>& selectedSystems, bool setupCompleted) const;

    /**
     * @brief Installs requested emulators and updates successful templates.
     *
     * @param settings Settings object to update with installed paths.
     * @param installRequests Systems and folders to install.
     * @return Result for each install attempt.
     */
    std::vector<InstallResult> installRequestedSystems(Settings& settings, const std::vector<InstallRequest>& installRequests) const;

private:
    /**
     * @brief Finds an editable template for a system.
     *
     * @param settings Settings object to search.
     * @param system System name to find.
     * @return Editable template, or `nullptr` when missing.
     */
    EmulatorTemplate* getEditableTemplate(Settings& settings, const std::string& system) const;

    /**
     * @brief Gets an existing template or creates a new one.
     *
     * @param settings Settings object to update.
     * @param system System name for the template.
     * @return Editable template for the system.
     */
    EmulatorTemplate& ensureTemplate(Settings& settings, const std::string& system) const;

    /**
     * @brief Gets the default launch arguments for a system.
     *
     * @param system System name to check.
     * @return Default argument string.
     */
    std::string getDefaultArgsForSystem(const std::string& system) const;

    /**
     * @brief Gets the expected emulator exe name for a system.
     *
     * @param system System name to check.
     * @return Expected executable name, or empty string if unknown.
     */
    QString getExeNameForSystem(const std::string& system) const;

    std::vector<std::string> supportedSystems = {
        "NES",
        "Super NES",
        "Game Boy Advance",
        "Game Boy",
        "Game Boy Color",
        "Nintendo 64"
    };
};

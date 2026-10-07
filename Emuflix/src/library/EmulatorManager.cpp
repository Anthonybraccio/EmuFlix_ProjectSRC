/**
 * @file EmulatorManager.cpp
 * @brief Implements emulator setup, install, and status helpers.
 */
#include "library/EmulatorManager.h"

#include <QFileInfo>
#include <QString>
#include <unordered_set>

/**
 * @brief Returns all systems supported by the app.
 *
 * @return Supported system names.
 */
const std::vector<std::string>& EmulatorManager::getSupportedSystems() const {
    return supportedSystems;
}


/**
 * @brief Searches settings for the template matching a system.
 *
 * @param settings Settings to search.
 * @param system System name to find.
 * @return Matching template, or `nullptr` when missing.
 */
const EmulatorTemplate* EmulatorManager::findTemplate(const Settings& settings, const std::string& system) const {
    for(const EmulatorTemplate& emulatorTemplate : settings.emulatorTemplates) {
        if(emulatorTemplate.system == system) {
            return &emulatorTemplate;
        }
    }

    return nullptr;
}   


/**
 * @brief Checks if a template points to a ready emulator.
 *
 * @param emulatorTemplate Template to inspect.
 * @return Emulator status for the template.
 */
EmulatorStatus EmulatorManager::getStatus(const EmulatorTemplate* emulatorTemplate) const {
    if(emulatorTemplate == nullptr) {
        return EmulatorStatus::NOTINSTALLED;
    }
    
    QString path = QString::fromStdString(emulatorTemplate -> emulatorPath).trimmed();

    if(path.isEmpty() == true) {
        return EmulatorStatus::NOTINSTALLED;
    }

    QFileInfo emulator(path);
    if(emulator.exists() == false || emulator.isFile() == false) {
        return EmulatorStatus::MISSING;
    }

    QString exeNameForSystem = getExeNameForSystem(emulatorTemplate -> system);
    if(exeNameForSystem.isEmpty() == false) {
        if(emulator.fileName().compare(exeNameForSystem, Qt::CaseInsensitive) != 0) {
            return EmulatorStatus::MISSING;
        }
    }

    return EmulatorStatus::READY;
}


/**
 * @brief Returns the systems selected for automatic install.
 *
 * @param settings Settings that store the selections.
 * @return Selected system names.
 */
const std::vector<std::string>& EmulatorManager::getInstallSystems(const Settings& settings) const {
    return settings.selectedInstallSystems;
}


/**
 * @brief Checks if setup has already been finished.
 *
 * @param settings Settings to inspect.
 * @return `true` when setup is complete.
 */
bool EmulatorManager::isSetupCompleted(const Settings& settings) const {
    return settings.setupCompleted;
}


/**
 * @brief Checks if a system is in the setup install list.
 *
 * @param settings Settings to inspect.
 * @param system System name to search for.
 * @return `true` when the system was selected.
 */
bool EmulatorManager::isSystemInstallSelected(const Settings& settings, const std::string& system) const {
    for(const std::string& selectedSystem : settings.selectedInstallSystems) {
        if(selectedSystem == system) {
            return true;
        }
    }

    return false;
}


/**
 * @brief Saves the setup selections into settings without duplicates.
 *
 * @param settings Settings object to update.
 * @param selectedSystems Systems selected by the user.
 * @param setupCompleted Whether the setup flow is complete.
 */
void EmulatorManager::applySetupSelections(Settings& settings, const std::vector<std::string>& selectedSystems, bool setupCompleted) const {
    settings.selectedInstallSystems.clear();

    std::unordered_set<std::string> knownSystem;

    for(const std::string& system : selectedSystems) {
        if(system.empty() == true) continue;
        if(knownSystem.contains(system) == true) continue;

        settings.selectedInstallSystems.push_back(system);
        knownSystem.insert(system);
    }

    settings.setupCompleted = setupCompleted;
}


/**
 * @brief Installs requested emulator packages and records successful paths.
 *
 * @param settings Settings object to update.
 * @param installRequests Install requests from setup or settings.
 * @return Results from each install attempt.
 */
std::vector<InstallResult> EmulatorManager::installRequestedSystems(Settings& settings, const std::vector<InstallRequest>& installRequests) const {
    std::vector<InstallResult> results;
    EmulatorInstaller installer;
    std::unordered_set<std::string> processedSystems;

    for(const InstallRequest& request : installRequests) {
        if(request.system.empty() == true) continue;
        if(processedSystems.contains(request.system) == true) continue;

        InstallResult result = installer.installSystem(request.system, request.destination);
        if(result.sucess == true) {
            EmulatorTemplate& emulatorTemplate = ensureTemplate(settings, request.system);
            emulatorTemplate.emulatorPath = result.executablePath;
            emulatorTemplate.arguments = getDefaultArgsForSystem(request.system);
            emulatorTemplate.enabled = true;
        }

        results.push_back(result);
        processedSystems.insert(request.system);
    }

    return results;
}


/**
 * @brief Finds a template that can be edited.
 *
 * @param settings Settings object to search.
 * @param system System name to find.
 * @return Pointer to the template, or `nullptr` if not found.
 */
EmulatorTemplate* EmulatorManager::getEditableTemplate(Settings& settings, const std::string& system) const {
    for(EmulatorTemplate& emulatorTemplate : settings.emulatorTemplates) {
        if(emulatorTemplate.system == system) {
            return &emulatorTemplate;
        }
    }

    return nullptr;
}


/**
 * @brief Returns a matching template, creating one if needed.
 *
 * @param settings Settings object to update.
 * @param system System name for the template.
 * @return Editable template for the system.
 */
EmulatorTemplate& EmulatorManager::ensureTemplate(Settings& settings, const std::string& system) const {
    EmulatorTemplate* exisitingTemplate = getEditableTemplate(settings, system);
    if(exisitingTemplate != nullptr) {
        return *exisitingTemplate;
    }

    EmulatorTemplate newTemplate;
    newTemplate.system = system;
    newTemplate.emulatorPath = "";
    newTemplate.arguments = "";
    newTemplate.enabled = true;

    settings.emulatorTemplates.push_back(newTemplate);
    return settings.emulatorTemplates.back();
}


/**
 * @brief Picks the default emulator arguments for a system.
 *
 * @param system System name to check.
 * @return Default arguments, or an empty string.
 */
std::string EmulatorManager::getDefaultArgsForSystem(const std::string& system) const {
    if(system == "NES" ||
       system == "Super NES" ||
       system == "Game Boy Advance" ||
       system == "Game Boy" ||
       system == "Game Boy Color" ||
       system == "Nintendo 64") {
        return "--fullscreen";
    }

    return "";
}


/**
 * @brief Finds the expected executable name for a system.
 *
 * @param system System name to check.
 * @return Expected exe name, or an empty string.
 */
QString EmulatorManager::getExeNameForSystem(const std::string& system) const {
    if(system == "NES" ||
       system == "Super NES" ||
       system == "Game Boy Advance" ||
       system == "Game Boy" ||
       system == "Game Boy Color") {
        return "Mesen.exe";
    }

    if(system == "Nintendo 64") {
        return "gopher64-windows-x86_64.exe";
    }

    return QString();
}

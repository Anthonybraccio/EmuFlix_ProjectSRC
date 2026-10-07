/**
 * @file SettingsWindow.h
 * @brief Declares the settings UI for configuring ROM folders and emulator paths.
 */
#pragma once

#include "core/Settings.h"
#include "settings/SettingsManager.h"
#include "library/EmulatorManager.h"

#include <string>
#include <vector>
#include <QWidget>

class QTabWidget;
class QListWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QCheckBox;
class QLabel;

/**
 * @brief Settings screen for managing ROM folders and emulator configuration.
 */
class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Creates the settings window and loads the persisted settings.
     *
     * @param settingsManager Manager used to load and save application settings.
     * @param emulatorManager Manager used to check and install emulators.
     * @param parent Optional parent widget.
     */
    explicit SettingsWindow(SettingsManager& settingsManager, EmulatorManager& emulatorManager, QWidget* parent = nullptr);

    /**
     * @brief Reloads settings from storage and updates the visible controls.
     */
    void loadSettings();

signals:
    /**
     * @brief Emitted when the user requests to return to the library screen.
     */
    void backClicked();

    /**
     * @brief Emitted when the user asks to install selected emulators.
     *
     * @param requests Systems and folders selected for install.
     */
    void installEmulatorsClicked(const std::vector<InstallRequest>& requests);

private slots:
    /**
     * @brief Opens the add-folder dialog and appends a valid new ROM directory.
     */
    void onAddClicked();

    /**
     * @brief Removes the currently selected ROM directory from the pending settings.
     */
    void onRemovedClicked();

    /**
     * @brief Persists the edited settings to storage.
     */
    void onSaveClicked();

    /**
     * @brief Discards unsaved edits and reloads the stored settings.
     */
    void onCancelClicked();

    /**
     * @brief Lets the user browse for an emulator executable path.
     */
    void browseForPath();

    /**
     * @brief Clears the current emulator executable path.
     */
    void clearEmulatorPath();

    /**
     * @brief Lets the user browse for an install folder.
     */
    void browseForInstall();

    /**
     * @brief Emits install requests for the selected emulator rows.
     */
    void onInstallClicked();

private:
    /**
     * @brief Widgets and data for one install row.
     */
    struct AddEmulatorRow {
        std::string system; ///< System name for this row.
        QCheckBox* checkBox = nullptr; ///< Checkbox that selects install.
        QLabel* status = nullptr; ///< Label showing install status.
        QLineEdit* path = nullptr; ///< Install folder path.
        QPushButton* browseButton = nullptr; ///< Button for choosing a folder.
    };

    /**
     * @brief Checks whether a ROM folder is already listed.
     *
     * @param insertedFolder Folder path to search for.
     * @return `true` if the folder is already present.
     */
    bool containsFolder(const std::string& insertedFolder) const;

    /**
     * @brief Rebuilds the ROM folder list widget from the current settings snapshot.
     */
    void refreshList();

    /**
     * @brief Loads the selected emulator template into the editor fields.
     */
    void loadTemplateEditor();

    /**
     * @brief Saves the template editor fields into the settings object.
     */
    void saveTemplateEditor();

    /**
     * @brief Finds the template index for a system.
     *
     * @param system System name to find.
     * @return Template index, or `-1` if not found.
     */
    int getTemplateIndexForSystem(const std::string& system) const;

    /**
     * @brief Handles the system dropdown changing in the template editor.
     */
    void systemTemplateChanged();

    /**
     * @brief Updates the visible status text for the selected template.
     */
    void updateTemplateStatus();

    /**
     * @brief Builds the default install folder for a system.
     *
     * @param system System name used in the folder path.
     * @return Default emulator install folder.
     */
    QString buildDefaultFolder(const std::string& system) const;

    /**
     * @brief Builds the install rows for all supported systems.
     */
    void loadAddEditor();

    /**
     * @brief Refreshes status labels for the install rows.
     */
    void refreshAddStatuses();

    /**
     * @brief Collects install requests from checked rows.
     *
     * @return Install requests selected by the user.
     */
    std::vector<InstallRequest> getInstallRequests();

    SettingsManager& settingsManager;
    EmulatorManager& emulatorManager;
    Settings settings;
    std::vector<AddEmulatorRow> emulatorRows;

    QTabWidget* tabWidget = nullptr;

    QListWidget* romFolderList = nullptr;
    QPushButton* addfolderButton = nullptr;
    QPushButton* removeFolderButton = nullptr;

    QComboBox* systemBox = nullptr;
    QLineEdit* templatePathLine = nullptr;
    QPushButton* templateBrowseButton = nullptr;
    QPushButton* templateClearButton = nullptr;
    QCheckBox* templateFullscreenBox = nullptr;
    QLabel* templateStatusLabel = nullptr;
    QLabel* installSelectionLabel = nullptr;
    QCheckBox* templateEnabledBox = nullptr;

    QPushButton* installButton = nullptr;
    QPushButton* saveButton = nullptr;
    QPushButton* cancelButton = nullptr;
};

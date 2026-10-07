/**
 * @file SettingsWindow.cpp
 * @brief Implements the settings UI for ROM folders and emulator path.
 */

#include "ui/SettingsWindow.h"
#include "ui/AddFolderDialog.h"

#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTabWidget> 
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QStandardPaths>

/**
 * @brief Builds the settings window and loads persisted settings.
 *
 * @param setManager Settings manager used for loading and saving.
 * @param emulatorManager Emulator manager used for status checks and installs.
 * @param parent Optional parent widget.
 */
SettingsWindow::SettingsWindow(SettingsManager& settingsManager, EmulatorManager& emulatorManager, QWidget* parent) 
    : QWidget(parent), settingsManager(settingsManager), emulatorManager(emulatorManager) {
    QVBoxLayout* settingsLayout = new QVBoxLayout(this);

    //Top bar for settings tabs
    QHBoxLayout* topBar = new QHBoxLayout();
    tabWidget = new QTabWidget(this);

    //Rom folder tab view for settings
    QWidget* romFolders = new QWidget(this);
    QVBoxLayout* romFoldersLayout = new QVBoxLayout(romFolders);
    QLabel* romLabel = new QLabel("ROM Folders", romFolders);

    //Rom folder button creation
    romFolderList = new QListWidget(romFolders);
    addfolderButton = new QPushButton("Add", romFolders);
    removeFolderButton = new QPushButton("Remove", romFolders);

    QHBoxLayout* romButtons = new QHBoxLayout();
    romButtons -> addWidget(addfolderButton);
    romButtons -> addWidget(removeFolderButton);
    romButtons -> addStretch();

    //Rom folder content view for settings
    romFoldersLayout -> addWidget(romLabel);
    romFoldersLayout -> addWidget(romFolderList, 1);
    romFoldersLayout -> addLayout(romButtons);

    //Emulator templates tab view for settings
    QWidget* emulatorTemplates = new QWidget(this);
    QVBoxLayout* emulatorTemplatesLayout = new QVBoxLayout(emulatorTemplates);

    QLabel* systemLabel = new QLabel("System:", emulatorTemplates);
    systemBox = new QComboBox(emulatorTemplates);
    
    for(const std::string& system : emulatorManager.getSupportedSystems()) {
        systemBox -> addItem(QString::fromStdString(system));
    }

    //Templates information boxes creation
    QLabel* templatePathLabel = new QLabel("Emulator Path:", emulatorTemplates);
    templatePathLine = new QLineEdit(emulatorTemplates);
    templatePathLine -> setReadOnly(true);

    templateBrowseButton = new QPushButton("Browse", emulatorTemplates);
    templateClearButton = new QPushButton("Clear", emulatorTemplates);

    QHBoxLayout* templatePathLayout = new QHBoxLayout();
    templatePathLayout -> addWidget(templatePathLine);
    templatePathLayout -> addWidget(templateBrowseButton);
    templatePathLayout -> addWidget(templateClearButton);

    QLabel* templateArguments = new QLabel("Arguments:", emulatorTemplates);
    templateFullscreenBox = new QCheckBox("Fullscreen", emulatorTemplates);

    QLabel* statusTitle = new QLabel("Status:", emulatorTemplates);
    templateStatusLabel = new QLabel("Not Installed", emulatorTemplates);
    templateStatusLabel -> setWordWrap(true);

    QLabel* installSelectionTitle = new QLabel("Setup Selection:", emulatorTemplates);
    installSelectionLabel = new QLabel("Not Selected", emulatorTemplates);
    installSelectionLabel -> setWordWrap(true);

    templateEnabledBox = new QCheckBox("Enabled", emulatorTemplates);

    //Template information added to settings
    emulatorTemplatesLayout -> addWidget(systemLabel);
    emulatorTemplatesLayout -> addWidget(systemBox);
    emulatorTemplatesLayout -> addWidget(templatePathLabel);
    emulatorTemplatesLayout -> addLayout(templatePathLayout);
    emulatorTemplatesLayout -> addWidget(templateArguments);
    emulatorTemplatesLayout -> addWidget(templateFullscreenBox);

    emulatorTemplatesLayout -> addWidget(statusTitle);
    emulatorTemplatesLayout -> addWidget(templateStatusLabel);
    emulatorTemplatesLayout -> addSpacing(10);
    emulatorTemplatesLayout -> addWidget(installSelectionTitle);
    emulatorTemplatesLayout -> addWidget(installSelectionLabel);

    emulatorTemplatesLayout -> addWidget(templateEnabledBox);
    emulatorTemplatesLayout -> addStretch();

    //add emulators tab view for settings
    QWidget* addEmulators = new QWidget(this);
    QVBoxLayout* addEmulatorsLayout = new QVBoxLayout(addEmulators);

    QLabel* addEmulatorsTitle = new QLabel("Add Emulators", addEmulators);
    QLabel* addEmulatorsDesc = new QLabel(
        "Select emulator systems you may want to install. "
        "Installed systems are shown here with their current status.",
        addEmulators
    );
    addEmulatorsDesc -> setWordWrap(true);

    addEmulatorsLayout -> addWidget(addEmulatorsTitle);
    addEmulatorsLayout -> addWidget(addEmulatorsDesc);

    for(const std::string& system : emulatorManager.getSupportedSystems()) {
        AddEmulatorRow row;

        row.system = system;
        row.checkBox = new QCheckBox(QString::fromStdString(system), addEmulators);

        row.status = new QLabel("Not Installed", addEmulators);
        row.status -> setMinimumWidth(120);

        row.path = new QLineEdit(addEmulators);
        row.path -> setReadOnly(true);
        row.path -> setText(buildDefaultFolder(system));
        row.path -> setEnabled(false);

        row.browseButton = new QPushButton("Browse", addEmulators);
        row.browseButton -> setEnabled(false);

        connect(row.checkBox, &QCheckBox::toggled, row.path, &QLineEdit::setEnabled);
        connect(row.checkBox, &QCheckBox::toggled, row.browseButton, &QPushButton::setEnabled);
        connect(row.browseButton, &QPushButton::clicked, this, &SettingsWindow::browseForInstall);

        QHBoxLayout* rowLayout = new QHBoxLayout();
        rowLayout -> addWidget(row.checkBox);
        rowLayout -> addWidget(row.status);
        rowLayout -> addWidget(row.path, 1);
        rowLayout -> addWidget(row.browseButton);

        addEmulatorsLayout -> addLayout(rowLayout);
        emulatorRows.push_back(row);
    }

    installButton = new QPushButton("Install Selected", addEmulators);
    addEmulatorsLayout -> addWidget(installButton);
    addEmulatorsLayout -> addStretch();

    //linking tabs to top bar
    tabWidget -> addTab(romFolders, "ROM Folders");
    tabWidget -> addTab(emulatorTemplates, "Emulator Templates");
    tabWidget -> addTab(addEmulators, "Add emulators");
    topBar -> addWidget(tabWidget, 1);

    //bottom bar buttons
    saveButton = new QPushButton("Save", this);
    cancelButton = new QPushButton("Cancel", this);
    
    QHBoxLayout* bottomSettingsScreen = new QHBoxLayout();
    bottomSettingsScreen -> addStretch();
    bottomSettingsScreen -> addWidget(saveButton);
    bottomSettingsScreen -> addWidget(cancelButton);

    settingsLayout -> addLayout(topBar, 1);
    settingsLayout -> addLayout(bottomSettingsScreen);

    //Connect buttons
    connect(addfolderButton, &QPushButton::clicked, this, &SettingsWindow::onAddClicked);
    connect(removeFolderButton, &QPushButton::clicked, this, &SettingsWindow::onRemovedClicked);
    
    connect(saveButton, &QPushButton::clicked, this, &SettingsWindow::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsWindow::onCancelClicked);
    
    connect(systemBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWindow::systemTemplateChanged);
    connect(templateBrowseButton, &QPushButton::clicked, this, &SettingsWindow::browseForPath);
    connect(templateClearButton, &QPushButton::clicked, this, &SettingsWindow::clearEmulatorPath);
    connect(templateFullscreenBox, &QCheckBox::toggled, this, &SettingsWindow::saveTemplateEditor);
    connect(templateEnabledBox, &QCheckBox::toggled, this, &SettingsWindow::updateTemplateStatus);

    connect(installButton, &QPushButton::clicked, this, &SettingsWindow::onInstallClicked);

    loadSettings();
}


/**
 * @brief Reloads persisted settings and updates UI controls.
 */
void SettingsWindow::loadSettings() {
    settings = settingsManager.load();
    refreshList();

    loadTemplateEditor();
    loadAddEditor();
}


/**
 * @brief Handles add-folder clicks by validating and appending a new path.
 */
void SettingsWindow::onAddClicked() {
    AddFolderDialog dialog(this);

    if(dialog.exec() != QDialog::Accepted) {
        return;
    }

    const std::string newFolderPath = dialog.getFolderPath();
    if(newFolderPath.empty() == true) {
        return;
    }

    QDir directory(QString::fromStdString(newFolderPath));
    if(directory.exists() == false){
        QMessageBox::warning(this, "Invalid Folder", "The new folder does not exist.");
        return;
    }

    if(containsFolder(newFolderPath) == true) {
        QMessageBox::warning(this, "Duplicate Folder", "The new folder is already listed as a ROM folder.");
        return;
    }

    settings.romDirectories.push_back(newFolderPath);
    refreshList();
}


/**
 * @brief Removes the selected ROM folder from the pending settings.
 */
void SettingsWindow::onRemovedClicked() {
    int selectedRomIndex = romFolderList -> currentRow();

    if(selectedRomIndex < 0 || selectedRomIndex >= (int)(settings.romDirectories.size())) {
        QMessageBox::warning(this, "No Selection", "Select a folder to remove first.");
        return;
    }

    settings.romDirectories.erase(settings.romDirectories.begin() + selectedRomIndex);
    refreshList();
}


/**
 * @brief Saves edited settings via the manager.
 */
void SettingsWindow::onSaveClicked() {
    saveTemplateEditor();

    settingsManager.save(settings);
    loadSettings();

    emit backClicked();
}


/**
 * @brief Discards pending edits and reloads persisted settings.
 */
void SettingsWindow::onCancelClicked() {
    loadSettings();

    emit backClicked();
}


/**
 * @brief Opens a file picker for the current emulator path.
 */
void SettingsWindow::browseForPath() {
    QString file = QFileDialog::getOpenFileName(
        this,
        "Select Emulator Executable",
        templatePathLine -> text(),
        "Executable Files (*.exe);;All Files (*)"
    );

    if(file.isEmpty() == true) return;

    templatePathLine -> setText(file);
    saveTemplateEditor();
    updateTemplateStatus();
}


/**
 * @brief Clears the emulator path field for the selected system.
 */
void SettingsWindow::clearEmulatorPath() {
    templatePathLine->clear();
    saveTemplateEditor();
    updateTemplateStatus();
}


/**
 * @brief Opens a folder picker for an install row.
 */
void SettingsWindow::browseForInstall() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if(clickedButton == nullptr) {
        return;
    }

    for(AddEmulatorRow& row : emulatorRows) {
        if(row.browseButton == clickedButton) {
            QString chosenFolder = QFileDialog::getExistingDirectory(
                this,
                "Choose Install Folder",
                row.path -> text()
            );

            if(chosenFolder.isEmpty() == false) {
                row.path -> setText(chosenFolder);
            }

            return;
        }
    }
}


/**
 * @brief Sends selected emulator install requests to the UI controller.
 */
void SettingsWindow::onInstallClicked() {
    std::vector<InstallRequest> requests = getInstallRequests();

    if(requests.empty() == true) {
        QMessageBox::warning(this, "No Emulators Selected", "Select at least one emulator to install.");
        return;
    }

    emit installEmulatorsClicked(requests);
}


/**
 * @brief Checks whether a folder path is already present in the list.
 *
 * @param insertedFolder Folder path to test.
 * @return `true` when the folder already exists.
 */
bool SettingsWindow::containsFolder(const std::string& insertedFolder) const {
    for(const std::string& folder : settings.romDirectories) {
        if(folder == insertedFolder) return true;
    }

    return false;
}


/**
 * @brief Repopulates the ROM folder list widget from current settings.
 */
void SettingsWindow::refreshList() {
    romFolderList -> clear();

    for(const std::string& folder: settings.romDirectories) {
        romFolderList -> addItem(QString::fromStdString(folder));
    }
}


/**
 * @brief Loads the selected system's emulator template into the form.
 */
void SettingsWindow::loadTemplateEditor() {
    if(systemBox == nullptr) {
        return;
    }

    std::string currentSystem = systemBox -> currentText().toStdString();

    int index = getTemplateIndexForSystem(currentSystem);
    if(index < 0) {
        templatePathLine -> clear();
        templateFullscreenBox -> setChecked(false);
        templateEnabledBox -> setChecked(true);
        updateTemplateStatus();

        return;
    }

    const EmulatorTemplate& emulatorTemplate = settings.emulatorTemplates[index];
    
    templatePathLine -> setText(QString::fromStdString(emulatorTemplate.emulatorPath));
    templateFullscreenBox -> setChecked(QString::fromStdString(emulatorTemplate.arguments).contains("--fullscreen"));
    templateEnabledBox -> setChecked(emulatorTemplate.enabled);
    updateTemplateStatus();
}


/**
 * @brief Saves the current emulator template editor fields.
 */
void SettingsWindow::saveTemplateEditor() {
    if(systemBox == nullptr) {
        return;
    }

    EmulatorTemplate emulatorTemplate;
    emulatorTemplate.system = systemBox -> currentText().toStdString();
    emulatorTemplate.emulatorPath = templatePathLine -> text().trimmed().toStdString();
    
    if(templateFullscreenBox -> isChecked() == true) {
        emulatorTemplate.arguments = "--fullscreen";
    }
    else {
        emulatorTemplate.arguments.clear();
    }

    emulatorTemplate.enabled = templateEnabledBox -> isChecked();

    int index = getTemplateIndexForSystem(emulatorTemplate.system);
    if(index < 0) {
        settings.emulatorTemplates.push_back(emulatorTemplate);
        updateTemplateStatus();

        return;
    }

    settings.emulatorTemplates[index].emulatorPath = emulatorTemplate.emulatorPath;
    settings.emulatorTemplates[index].arguments = emulatorTemplate.arguments;
    settings.emulatorTemplates[index].enabled = emulatorTemplate.enabled;
    updateTemplateStatus();
}


/**
 * @brief Finds the saved template for one system.
 *
 * @param system System name to find.
 * @return Index of the template, or `-1` when missing.
 */
int SettingsWindow::getTemplateIndexForSystem(const std::string& system) const {
    for(int i = 0; i< (int)settings.emulatorTemplates.size(); i++) {
        if(settings.emulatorTemplates[i].system == system) {
            return i;
        }
    }

    return -1;
}


/**
 * @brief Loads the newly selected system into the editor.
 */
void SettingsWindow::systemTemplateChanged() {
    loadTemplateEditor();
}


/**
 * @brief Updates labels that tell the user if an emulator is ready.
 */
void SettingsWindow::updateTemplateStatus() {
    if(templateStatusLabel == nullptr || installSelectionLabel == nullptr) {
        return;
    }

    EmulatorTemplate emulatorTemplate;
    emulatorTemplate.system = systemBox -> currentText().toStdString();
    emulatorTemplate.emulatorPath = templatePathLine -> text().trimmed().toStdString();

    if(templateFullscreenBox -> isChecked() == true) {
        emulatorTemplate.arguments = "--fullscreen";
    }
    else {
        emulatorTemplate.arguments.clear();
    }

    emulatorTemplate.enabled = templateEnabledBox -> isChecked();

    bool selectedInSetup = emulatorManager.isSystemInstallSelected(settings, emulatorTemplate.system);

    if(selectedInSetup == true) {
        installSelectionLabel -> setText("Selected in setup");
    }
    else {
        installSelectionLabel -> setText("Not selected in setup");
    }

    EmulatorStatus status = emulatorManager.getStatus(&emulatorTemplate);

    if(status == EmulatorStatus::READY) {
        templateStatusLabel -> setText("Ready - emulator path is valid.");
        return;
    }
    if(status == EmulatorStatus::MISSING) {
        templateStatusLabel -> setText("Missing - stored emulator path was not found. Update the path.");
        return;
    }

    templateStatusLabel -> setText("Not Installed - no emulator path is configured yet.");
    
}


/**
 * @brief Builds a default folder path for installing an emulator.
 *
 * @param system System name used in the folder path.
 * @return Default install folder path.
 */
QString SettingsWindow::buildDefaultFolder(const std::string& system) const {
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir dir(localPath);

    return dir.filePath(QString("EmuFlix/emulators/%1").arg(QString::fromStdString(system)));
}


/**
 * @brief Resets install rows for supported systems.
 */
void SettingsWindow::loadAddEditor() {
    for(AddEmulatorRow& row : emulatorRows) {
        row.checkBox -> setChecked(false);
        row.path -> setText(buildDefaultFolder(row.system));
        row.path -> setEnabled(false);
        row.browseButton -> setEnabled(false);
    }

    refreshAddStatuses();
}


/**
 * @brief Refreshes install status labels and default paths.
 */
void SettingsWindow::refreshAddStatuses() {
    for(AddEmulatorRow& row : emulatorRows) {
        const EmulatorTemplate* emulatorTemplate = emulatorManager.findTemplate(settings, row.system);
        EmulatorStatus status = emulatorManager.getStatus(emulatorTemplate);

        if(status == EmulatorStatus::READY) {
            row.status -> setText("Installed");
            row.checkBox -> setEnabled(false);
        }
        else {
            row.status -> setText("Not Installed");
            row.checkBox -> setEnabled(true);
        }
    }
}


/**
 * @brief Builds install requests from checked rows.
 *
 * @return Requested emulator installs.
 */
std::vector<InstallRequest> SettingsWindow::getInstallRequests() {
    std::vector<InstallRequest> requests;

    for(const AddEmulatorRow& row : emulatorRows) {
        if(row.checkBox ->  isChecked() == false) continue;

        InstallRequest request;

        request.system = row.system;
        request.destination = row.path -> text().trimmed().toStdString();
        requests.push_back(request);
    }

    return requests;
}

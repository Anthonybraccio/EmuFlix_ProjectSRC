/**
 * @file SetupWindow.cpp
 * @brief Implements the first-time emulator setup dialog.
 */
#include "ui/SetupWindow.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QStandardPaths>

/**
 * @brief Builds a setup dialog with one row for each supported system.
 *
 * @param supportedSystems Systems the app can install.
 * @param parent Optional parent widget.
 */
SetupWindow::SetupWindow(const std::vector<std::string>& supportedSystems, QWidget* parent) 
    : QDialog(parent) {
    setWindowTitle("EmulatorSetup");
    resize(700, 360);
    setModal(true);

    QVBoxLayout* windowLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Choose which emulator systems to set up.", this);
    title -> setWordWrap(true);
    QLabel* info = new QLabel(
        "You can continue without selecting any systems. "
        "Games will need the emulators installed before they can launch.",
        this
    );
    info -> setWordWrap(true);

    windowLayout -> addWidget(title);
    windowLayout -> addWidget(info);

    for(const std::string& system : supportedSystems) {
        SystemRow row;
        row.system = system;
        row.checkBox = new QCheckBox(QString::fromStdString(system), this);

        row.path = new QLineEdit(this);
        row.path -> setText(buildDefaultFolder(system));
        row.path -> setReadOnly(true);
        row.path -> setEnabled(false);
        
        row.browseButton = new QPushButton("Browse", this);
        row.browseButton -> setEnabled(false);

        connect(row.checkBox, &QCheckBox::toggled, row.path, &QLineEdit::setEnabled);
        connect(row.checkBox, &QCheckBox::toggled, row.browseButton, &QPushButton::setEnabled);
        connect(row.browseButton, &QPushButton::clicked, this, &SetupWindow::browseFolderClicked);

        QHBoxLayout* rowLayout = new QHBoxLayout();
        rowLayout -> addWidget(row.checkBox);
        rowLayout -> addWidget(row.path, 1);
        rowLayout -> addWidget(row.browseButton);

        windowLayout -> addLayout(rowLayout);
        systemRows.push_back(row);
    }

    windowLayout -> addStretch();

    cancelButton = new QPushButton("Cancel", this);
    finishButton = new QPushButton("Finish", this);

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons -> addStretch();
    buttons -> addWidget(finishButton);
    buttons -> addWidget(cancelButton);

    windowLayout ->  addLayout(buttons);

    connect(finishButton, &QPushButton::clicked, this, &SetupWindow::finishedClicked);
    connect(cancelButton, &QPushButton::clicked, this, &SetupWindow::reject);
}


/**
 * @brief Collects selected systems with their chosen install folders.
 *
 * @return Install selections from checked rows.
 */
std::vector<SetupInstallSelection> SetupWindow::getInstallSelections() const {
    std::vector<SetupInstallSelection> systemSelections;

    for(const SystemRow& systemRow : systemRows) {
        if(systemRow.checkBox != nullptr && systemRow.checkBox -> isChecked() == true) {
            SetupInstallSelection selection;

            selection.system = systemRow.system;
            selection.folder = systemRow.path -> text().toStdString();

            systemSelections.push_back(selection);
        }
    }

    return systemSelections;
}


/**
 * @brief Closes the dialog with an accepted result.
 */
void SetupWindow::finishedClicked() {
    accept();
}


/**
 * @brief Lets the user browse for an install folder on the clicked row.
 */
void SetupWindow::browseFolderClicked() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if(clickedButton == nullptr) {
        return;
    }

    for(SystemRow& row : systemRows) {
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
 * @brief Builds a default emulator folder under the app data location.
 *
 * @param system System name used in the folder path.
 * @return Default install folder for that system.
 */
QString SetupWindow::buildDefaultFolder(const std::string& system) const {
    QString localPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QDir dir(localPath);

    return dir.filePath(QString("EmuFlix/emulators/%1").arg(QString::fromStdString(system)));
}

/**
 * @brief Collects just the selected system names.
 *
 * @return Names of checked systems.
 */
std::vector<std::string> SetupWindow::getSelectedSystems() const {
    std::vector<std::string> systems;

    for(const SystemRow& row : systemRows) {
        if(row.checkBox != nullptr && row.checkBox->isChecked() == true) {
            systems.push_back(row.system);
        }
    }

    return systems;
}

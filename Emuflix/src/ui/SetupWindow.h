/**
 * @file SetupWindow.h
 * @brief Declares the first-time emulator setup dialog.
 */
#pragma once

#include <QDialog>
#include <string>
#include <vector>

class QCheckBox;
class QLineEdit;
class QPushButton;

/**
 * @brief Stores the system and folder picked during setup.
 */
struct SetupInstallSelection {
    std::string system; ///< Console system selected for install.
    std::string folder; ///< Folder where the emulator should be installed.
};

/**
 * @brief Dialog that lets the user choose emulator installs during setup.
 */
class SetupWindow : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Builds the setup dialog for all supported systems.
     *
     * @param supportedSystems Systems that can be installed.
     * @param parent Optional parent widget.
     */
    SetupWindow(const std::vector<std::string>& supportedSystems, QWidget* parent = nullptr);

    /**
     * @brief Gets install selections with their destination folders.
     *
     * @return Selected install systems and folders.
     */
    std::vector<SetupInstallSelection> getInstallSelections() const;

    /**
     * @brief Gets only the names of selected systems.
     *
     * @return Selected system names.
     */
    std::vector<std::string> getSelectedSystems() const;
private slots:
    /**
     * @brief Accepts the setup dialog when the user finishes.
     */
    void finishedClicked();

    /**
     * @brief Opens a folder picker for the clicked system row.
     */
    void browseFolderClicked();

private:
    /**
     * @brief Widgets and data for one system row in the dialog.
     */
    struct SystemRow {
        std::string system; ///< System name shown on this row.
        QCheckBox* checkBox = nullptr; ///< Checkbox used to select the system.
        QLineEdit* path = nullptr; ///< Folder path field for this system.
        QPushButton* browseButton = nullptr; ///< Button that opens the folder picker.
    };

    /**
     * @brief Builds the default install folder for a system.
     *
     * @param system System name to place in the folder path.
     * @return Default folder path for that system.
     */
    QString buildDefaultFolder(const std::string& system) const;

    std::vector<SystemRow> systemRows;
    QPushButton* cancelButton = nullptr;
    QPushButton* finishButton = nullptr;
};

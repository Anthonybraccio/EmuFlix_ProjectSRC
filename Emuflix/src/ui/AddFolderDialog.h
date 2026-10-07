/**
 * @file AddFolderDialog.h
 * @brief Declares the dialog used to add ROM folders.
 */
#pragma once

#include <QDialog>
#include <string>

class QLineEdit;
class QPushButton;

/**
 * @brief Modal dialog used to collect a ROM folder path from the user.
 */
class AddFolderDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Creates the add-folder dialog.
     *
     * @param parent Optional parent widget.
     */
    explicit AddFolderDialog(QWidget* parent = nullptr);

    /**
     * @brief Returns the folder path entered by the user.
     *
     * @return Trimmed folder path as a standard string.
     */
    std::string getFolderPath() const;

private slots:
    /**
     * @brief Opens a directory picker and stores the selected folder path.
     */
    void onBrowseClicked();

    /**
     * @brief Validates the current input and accepts the dialog when valid.
     */
    void onAcceptClicked();

private:
    QLineEdit* folderPath = nullptr;
    QPushButton* browseButton = nullptr;
    QPushButton* okButton = nullptr;
    QPushButton* cancelButton = nullptr;
};

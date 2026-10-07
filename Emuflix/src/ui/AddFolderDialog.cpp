/**
 * @file AddFolderDialog.cpp
 * @brief Implements the add ROM folder dialog.
 */

#include "ui/AddFolderDialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

/**
 * @brief Builds the add-folder dialog UI and wires signals.
 *
 * @param parent Optional parent widget.
 */
AddFolderDialog::AddFolderDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Add ROM Folder");
    resize(500, 140);

    //Window layout for popup
    QVBoxLayout* windowLayout = new QVBoxLayout(this);

    //Window content for popup
    QLabel* label = new QLabel("Folder Path:", this);
    folderPath = new QLineEdit(this);
    browseButton = new QPushButton("Browse", this);

    QHBoxLayout* folderPathLayout = new QHBoxLayout();
    folderPathLayout -> addWidget(folderPath);
    folderPathLayout -> addWidget(browseButton);

    //bottom button bar for popup
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    QHBoxLayout* buttons = new QHBoxLayout();
    buttons -> addStretch();
    buttons -> addWidget(okButton);
    buttons -> addWidget(cancelButton);

    //connect layouts and buttons for popup
    windowLayout -> addWidget(label);
    windowLayout -> addLayout(folderPathLayout);
    windowLayout -> addStretch();
    windowLayout -> addLayout(buttons);

    connect(browseButton, &QPushButton::clicked, this, &AddFolderDialog::onBrowseClicked);
    connect(okButton, &QPushButton::clicked, this, &AddFolderDialog::onAcceptClicked);
    connect(cancelButton, &QPushButton::clicked, this, &AddFolderDialog::reject);
}


/**
 * @brief Returns the trimmed folder path entered by the user.
 *
 * @return Folder path as a standard string.
 */
std::string AddFolderDialog::getFolderPath() const{
    return folderPath->text().trimmed().toStdString();
}


/**
 * @brief Opens a folder picker and copies the selection into the input box.
 */
void AddFolderDialog::onBrowseClicked(){
    QString newFolder = QFileDialog::getExistingDirectory(this, "Select ROM Folder");
    
    if (!newFolder.isEmpty()) {
        folderPath->setText(newFolder);
    }
}


/**
 * @brief Validates input and accepts the dialog when a folder is provided.
 */
void AddFolderDialog::onAcceptClicked(){
    if (folderPath->text().trimmed().isEmpty()){
        QMessageBox::warning(this, "", "Please Select a folder");
        return;
    }

    accept();
}

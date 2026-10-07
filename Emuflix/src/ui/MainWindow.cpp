/**
 * @file MainWindow.cpp
 * @brief Implements the main application window UI.
 */

#include "ui/MainWindow.h"

#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSignalBlocker>
#include <QCheckBox>
#include <QScrollArea>
#include <QSizePolicy>
#include <QGridLayout>
#include <QLayoutItem>
#include <QList>
#include <QScrollBar>
#include <algorithm>
#include <QTimer>

/**
 * @brief Builds the main window layout and connects UI signals.
 *
 * @param parent Optional parent widget.
 */
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("EmuFlix");
    setFocusPolicy(Qt::StrongFocus);
    resize(1600, 1000);
    //Root window and window stack
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainWindowLayout = new QVBoxLayout(centralWidget);
    
    appStack = new QStackedWidget(centralWidget);

    //Library window
    libraryWindow = new QWidget(centralWidget);
    QVBoxLayout* libraryWindowLayout = new QVBoxLayout(libraryWindow);

    //button bar for library
    QHBoxLayout* buttonBar = new QHBoxLayout();
    settingsButton = new QPushButton("Settings", libraryWindow);
    settingsButton -> setMinimumHeight(38);
    settingsButton -> setMinimumWidth(110);
    settingsButton -> setStyleSheet(
        "QPushButton {"
        "  background-color: #2a2a2a;"
        "  color: white;"
        "  border: 1px solid #444444;"
        "  border-radius: 8px;"
        "  padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #353535;"
        "}"
    );

    favouriteButton = new QPushButton("Favourite", libraryWindow);
    favouriteButton -> setMinimumHeight(38);
    favouriteButton -> setMinimumWidth(120);
    favouriteButton -> setStyleSheet(
        "QPushButton {"
        "  background-color: #2a2a2a;"
        "  color: white;"
        "  border: 1px solid #444444;"
        "  border-radius: 8px;"
        "  padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #353535;"
        "}"
        "QPushButton:disabled {"
        "  color: #777777;"
        "  background-color: #1d1d1d;"
        "}"
    );

    launchButton = new QPushButton("Launch", libraryWindow);
    launchButton -> setMinimumHeight(38);
    launchButton -> setMinimumWidth(120);
    launchButton -> setStyleSheet(
        "QPushButton {"
        "  background-color: #e50914;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 8px 18px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #faa7ab;"
        "}"
    );

    favouriteButton -> setEnabled(false);

    buttonBar -> addWidget(settingsButton);
    buttonBar -> addStretch();
    buttonBar -> addWidget(favouriteButton);
    buttonBar -> addWidget(launchButton);
    libraryWindowLayout -> addLayout(buttonBar);

    //Browse bar for library
    QHBoxLayout* browseBar = new QHBoxLayout();
    browseBar -> setContentsMargins(0, 2, 0, 8);
    browseBar -> setSpacing(10);

    searchBox = new QLineEdit(libraryWindow);
    searchBox -> setMinimumHeight(38);
    searchBox -> setStyleSheet(
        "QLineEdit {"
        "  background-color: #1f1f1f;"
        "  color: white;"
        "  border: 1px solid #444444;"
        "  border-radius: 8px;"
        "  padding: 0 12px;"
        "}"
    );

    sortDropdown = new QComboBox(libraryWindow);
    sortDropdown -> setMinimumHeight(38);
    sortDropdown -> setStyleSheet(
        "QComboBox {"
        "  background-color: #1f1f1f;"
        "  color: white;"
        "  border: 1px solid #444444;"
        "  border-radius: 8px;"
        "  padding: 0 12px;"
        "  min-width: 150px;"
        "}"
    );

    favouriteOnlyBox = new QCheckBox("Favourites only", libraryWindow);
    favouriteOnlyBox -> setStyleSheet(
        "QCheckBox {"
        "  color: white;"
        "  spacing: 8px;"
        "}"
    );

    searchBox -> setPlaceholderText("Search by title...");
    sortDropdown -> addItem("Ascending (A-Z)");
    sortDropdown -> addItem("Descending (Z-A)");

    browseBar -> addWidget(searchBox, 1);
    browseBar -> addWidget(favouriteOnlyBox);
    browseBar -> addWidget(sortDropdown);
    libraryWindowLayout -> addLayout(browseBar);

    //content view for library
    QVBoxLayout* libraryContent = new QVBoxLayout();
    shelvesContent = new QWidget();

    shelvesLayout = new QVBoxLayout(shelvesContent);
    shelvesLayout -> setContentsMargins(0, 0, 0, 0);
    shelvesLayout -> setSpacing(18);

    shelvesScrollArea = new QScrollArea(libraryWindow);
    shelvesScrollArea -> setWidgetResizable(true);

    shelvesScrollArea -> setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    shelvesScrollArea -> setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    shelvesScrollArea -> setFrameShape(QFrame::NoFrame);
    shelvesScrollArea -> setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QScrollBar:vertical {"
        "  background: #141414;"
        "  width: 10px;"
        "  margin: 2px 2px 2px 2px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #3a3a3a;"
        "  min-height: 30px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #555555;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "  background: none;"
        "  border: none;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "  background: none;"
        "}"
    );

    shelvesScrollArea -> setWidget(shelvesContent);

    details = new QFrame(libraryWindow);
    details -> setFrameShape(QFrame::NoFrame);
    details -> setStyleSheet(
        "QFrame {"
        "  background-color: #141414;"
        "  border: 1px solid #2f2f2f;"
        "  border-radius: 12px;"
        "}"
        "QLabel {"
        "  background: transparent;"
        "  border: none;"
        "}"
    );

    QVBoxLayout* detailsLayout = new QVBoxLayout(details);
    detailsLayout -> setContentsMargins(12, 10, 10, 10);
    detailsLayout -> setSpacing(6);

    QHBoxLayout* topDetailsLayout = new QHBoxLayout();
    topDetailsLayout -> setSpacing(16);

    //left column for details
    QVBoxLayout* leftColumn = new QVBoxLayout();
    leftColumn -> setSpacing(2);

    QLabel* titleLabel = new QLabel("Title:", details);
    titleValue = new QLabel("Select a game.", details);
    titleValue -> setWordWrap(true);

    QLabel* systemLabel = new QLabel("System:", details);
    systemValue = new QLabel("-", details);
    systemValue -> setWordWrap(true);

    leftColumn -> addWidget(titleLabel);
    leftColumn -> addWidget(titleValue);
    leftColumn -> addSpacing(5);
    leftColumn -> addWidget(systemLabel);
    leftColumn -> addWidget(systemValue);

    //right column for details
    QVBoxLayout* rightColumn = new QVBoxLayout();
    rightColumn -> setSpacing(5);

    QLabel* favouriteLabel= new QLabel("Favourite:", details);
    favouriteValue = new QLabel("-", details);
    favouriteValue -> setWordWrap(true);

    QLabel* lastPlayedLabel = new QLabel("Last Played:", details);
    lastPlayedValue = new QLabel("-", details);
    lastPlayedValue -> setWordWrap(true);

    QLabel* timePlayedLabel = new QLabel("Time Played:", details);
    timePlayedValue = new QLabel("-", details);
    timePlayedValue -> setWordWrap(true);

    rightColumn -> addWidget(favouriteLabel);
    rightColumn -> addWidget(favouriteValue);
    rightColumn -> addSpacing(5);
    rightColumn -> addWidget(lastPlayedLabel);
    rightColumn -> addWidget(lastPlayedValue);
    rightColumn -> addSpacing(5);
    rightColumn -> addWidget(timePlayedLabel);
    rightColumn -> addWidget(timePlayedValue);

    topDetailsLayout -> addLayout(leftColumn, 1);
    topDetailsLayout -> addLayout(rightColumn, 1);

    //separator and bottom for details
    QFrame* separator = new QFrame(details);
    separator -> setFrameShape(QFrame::HLine);
    separator -> setFrameShadow(QFrame::Plain);
    separator -> setStyleSheet("color: #2f2f2f; border: none; background-color: #2f2f2f; min-height: 1px; max-height: 1px;");

    QVBoxLayout* bottomDetailsLayout = new QVBoxLayout();
    bottomDetailsLayout -> setSpacing(6);

    QLabel* romPathLabel = new QLabel("ROM Path:", details);
    romPathValue = new QLabel("-", details);
    romPathValue -> setWordWrap(true);

    //style polish labels
    titleLabel -> setStyleSheet("font-weight: bold; color: #ffffff;");
    systemLabel -> setStyleSheet("color: #9a9a9a; font-size: 11px; text-transform: uppercase;");
    favouriteLabel -> setStyleSheet("color: #9a9a9a; font-size: 11px; text-transform: uppercase;");
    lastPlayedLabel -> setStyleSheet("color: #9a9a9a; font-size: 11px; text-transform: uppercase;");
    timePlayedLabel -> setStyleSheet("color: #9a9a9a; font-size: 11px; text-transform: uppercase;");
    romPathLabel -> setStyleSheet("color: #9a9a9a; font-size: 11px; text-transform: uppercase;");

    //style polish text
    titleValue -> setStyleSheet("color: white; font-weight: bold; font-size: 20px;");
    systemValue -> setStyleSheet("color: #d0d0d0; font-weight: bold;");
    favouriteValue -> setStyleSheet("color: #d0d0d0; font-weight: bold;");
    lastPlayedValue -> setStyleSheet("color: #d0d0d0; font-weight: bold;");
    timePlayedValue -> setStyleSheet("color: #d0d0d0; font-weight: bold;");
    romPathValue -> setStyleSheet("color: #d0d0d0; font-weight: bold;");

    bottomDetailsLayout -> addWidget(romPathLabel);
    bottomDetailsLayout -> addWidget(romPathValue);

    detailsLayout -> addLayout(topDetailsLayout);
    detailsLayout -> addWidget(separator);
    detailsLayout -> addLayout(bottomDetailsLayout);

    libraryContent -> setContentsMargins(0, 0, 0, 0);
    libraryContent -> setSpacing(16);

    libraryContent -> addWidget(shelvesScrollArea, 1);
    libraryContent -> addWidget(details, 0);
    libraryWindowLayout -> addLayout(libraryContent);

    //Library window linking to main window.
    appStack -> addWidget(libraryWindow);
    mainWindowLayout -> addWidget(appStack);
    setCentralWidget(centralWidget);

    //connecting signals
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::settingsClicked);
    connect(favouriteButton, &QPushButton::clicked, this, &MainWindow::favouriteClicked);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::LaunchClicked);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::searchChanged);
    connect(favouriteOnlyBox, &QCheckBox::checkStateChanged, this, &MainWindow::favouriteOnlyChanged);
    connect(sortDropdown, &QComboBox::currentIndexChanged, this, &MainWindow::sortChanged);

    appStack -> setCurrentWidget(libraryWindow);
}


/**
 * @brief Replaces the visible game list with a new collection.
 *
 * @param newGames Games to display.
 */
void MainWindow::setGames(const std::vector<Game>& newGames) {
    std::string selectedGameId = getSelectedGameId();

    currentGames = newGames;
    selectedIndex = -1;

    for(int i = 0; i < (int)currentGames.size(); i++) {
        if(currentGames[i].id == selectedGameId) {
            selectedIndex = i;
            break;
        }
    }

    if(selectedIndex == -1 && currentGames.empty() == false) {
        selectedIndex = 0;
    }

    rebuildRowShelves();

    if(syncFocusToSelectedGame() == false) {
        focusFirstAvailableCard();
    }

    QTimer::singleShot(0, this, [this]() {
        ensureFocusedCardVisible();
        setFocus();
    });

    std::string currentGameId = getSelectedGameId();
    if(currentGameId.empty() == false) {
        favouriteButton -> setEnabled(true);
        emit gameSelectionChanged(currentGameId);
    }
    else {
        favouriteButton -> setEnabled(false);
        favouriteButton -> setText("Favourite");

        QString searchText = QString::fromStdString(getSearchText()).trimmed();
        if(searchText.isEmpty() == false) {
            setDetailsText("Title: No games match your search.\nSystem: -\nFavourite: -\nLast Played: -\nTime Played: -\nROM Path: -");
        }
        else {
            setDetailsText("Title: Select a game.\nSystem: -\nFavourite: -\nLast Played: -\nTime Played: -\nROM Path: -");
        }
    }
}
    

/**
 * @brief Returns the identifier for the currently selected game.
 *
 * @return Selected game id, or empty string when none selected.
 */
std::string MainWindow::getSelectedGameId() const {
    if(selectedIndex < 0 || selectedIndex >= (int)currentGames.size()) {
        return "";
    }

    return currentGames[selectedIndex].id;
}


/**
 * @brief Updates the details area text.
 *
 * @param text Rich or plain text to display.
 */
void MainWindow::setDetailsText(const QString& text) {
    titleValue -> setText("Select a game.");
    systemValue -> setText("-");
    favouriteValue -> setText("-");
    lastPlayedValue-> setText("-");
    timePlayedValue-> setText("-");
    romPathValue -> setText("-");

    const QStringList textLines = text.split('\n', Qt::SkipEmptyParts);

    for(const QString& line : textLines) {
        if(line.startsWith("Title: ")) {
            titleValue -> setText(line.mid(QString("Title: ").length()).trimmed());
        }
        else if(line.startsWith("System: ")) {
            systemValue -> setText(line.mid(QString("System: ").length()).trimmed());
        }
        else if(line.startsWith("Favourited: ")) {
            favouriteValue -> setText(line.mid(QString("Favourited: ").length()).trimmed());
        }
        else if(line.startsWith("Last Played: ")) {
            lastPlayedValue -> setText(line.mid(QString("Last Played: ").length()).trimmed());
        }
        else if(line.startsWith("Time Played: ")) {
            timePlayedValue-> setText(line.mid(QString("Time Played: ").length()).trimmed());
        }
        else if(line.startsWith("ROM Path: ")) {
            romPathValue -> setText(line.mid(QString("ROM Path: ").length()).trimmed());
        }
    }
}


/**
 * @brief Updates the favourite button label to reflect current state.
 *
 * @param favourite Whether the current game is favourited.
 */
void MainWindow::setFavouriteButtonText(bool favourite) {
    if(favouriteButton == nullptr) {
        return;
    }

    favouriteButton -> setEnabled(true);
    if(favourite == true) {
        favouriteButton -> setText("Unfavourite");
    }
    else {
        favouriteButton -> setText("Favourite");
    }
}


/**
 * @brief Registers the settings window with the stacked widget.
 *
 * @param newSettingsWindow Settings window instance.
 */
void MainWindow::setSettingsWindow(SettingsWindow* newSettingsWindow) {
    settingsWindow = newSettingsWindow;

    if(settingsWindow != nullptr && appStack -> indexOf(settingsWindow) == -1) {
        appStack -> addWidget(settingsWindow);
    }
}


/**
 * @brief Switches the stacked view to the library screen.
 */
void MainWindow::showLibrary() {
    appStack -> setCurrentWidget(libraryWindow);
}


/**
 * @brief Switches the stacked view to the settings screen.
 */
void MainWindow::showSettingsWindow() {
    if(settingsWindow != nullptr) {
        appStack -> setCurrentWidget(settingsWindow);
    }
}


/**
 * @brief Displays a modal error dialog.
 *
 * @param message Error text to display.
 */
void MainWindow::showError(const QString& message) {
    if(activeErrorBox != nullptr) {
        activeErrorBox->setText(message);
        activeErrorBox->raise();
        activeErrorBox->activateWindow();
        return;
    }

    activeErrorBox = new QMessageBox(
        QMessageBox::Critical,
        "EmuFlix - Error",
        message,
        QMessageBox::Ok,
        this
    );

    activeErrorBox -> setDefaultButton(QMessageBox::Ok);
    activeErrorBox -> setEscapeButton(QMessageBox::Ok);
    activeErrorBox -> setAttribute(Qt::WA_DeleteOnClose, true);

    connect(activeErrorBox, &QMessageBox::finished, this, [this](int) {
        activeErrorBox = nullptr;
        
        setFocus();
    });

    activeErrorBox->open();
}


/**
 * @brief Returns the current search text.
 *
 * @return Raw search box contents.
 */
std::string MainWindow::getSearchText() const {
    if(searchBox == nullptr) {
        return "";
    }

    return searchBox->text().toStdString();
}


/**
 * @brief Returns the selected sort option from the dropdown.
 *
 * @return Corresponding `SortMode` value.
 */
SortMode MainWindow::getSelectedSortOption() const {
    if (sortDropdown == nullptr) {
        return SortMode::TitleAscending;
    }

    if (sortDropdown -> currentIndex() == 1) {
        return SortMode::TitleDescending;
    }

    return SortMode::TitleAscending;
}


/**
 * @brief Indicates whether the favourites-only filter is active.
 *
 * @return `true` when only favourites should be shown.
 */
bool MainWindow::favouriteOnlyEnabled() const {
    if(favouriteOnlyBox == nullptr) {
        return false;
    }

    return favouriteOnlyBox -> isChecked();
}


/**
 * @brief Moves focused card selection one position left.
 *
 * @return `true` when the focus moved.
 */
bool MainWindow::moveFocusLeft() {
    if(hasFocusedCard() == false || focusedColumn == 0) {
        return false;
    }
    
    focusedColumn--;
    preferredColumn = focusedColumn;

    selectFocusedGame();
    ensureFocusedCardVisible();

    return true;
}


/**
 * @brief Moves focused card selection one position right.
 *
 * @return `true` when the focus moved.
 */
bool MainWindow::moveFocusRight() {
    if(hasFocusedCard() == false) {
        return false;
    }
    
    const ShelfRow& row = shelfRows[focusedRow];
    if(focusedColumn >= (int)row.gameIndexes.size() - 1) {
        return false;
    }
    
    focusedColumn++;
    preferredColumn = focusedColumn;

    selectFocusedGame();
    ensureFocusedCardVisible();
    
    return true;
}


/**
 * @brief Moves focus to the nearest shelf above.
 *
 * @return `true` when the focus moved.
 */
bool MainWindow::moveFocusUp() {
    if(hasFocusedCard() == false) {
        return false;
    }

    for(int rowIndex = focusedRow - 1; rowIndex >= 0; rowIndex--) {
        const ShelfRow& row = shelfRows[rowIndex];

        if(row.gameIndexes.empty() == true) {
            continue;
        }
        
        focusedRow = rowIndex;
        focusedColumn = std::min(preferredColumn, (int)row.gameIndexes.size() - 1);
        
        selectFocusedGame();
        ensureFocusedCardVisible();

        return true;
    }

    return false;
}


/**
 * @brief Moves focus to the nearest shelf below.
 *
 * @return `true` when the focus moved.
 */
bool MainWindow::moveFocusDown() {
    if(hasFocusedCard() == false) {
        return false;
    }

    for(int rowIndex = focusedRow + 1; rowIndex < (int)shelfRows.size(); rowIndex++) {
        const ShelfRow& row = shelfRows[rowIndex];

        if(row.gameIndexes.empty() == true) {
            continue;
        }
        
        focusedRow = rowIndex;
        focusedColumn = std::min(preferredColumn, (int)row.gameIndexes.size() - 1);
        
        selectFocusedGame();
        ensureFocusedCardVisible();

        return true;
    }

    return false;
}


/**
 * @brief Cycles the sort dropdown toward the left controller direction.
 */
void MainWindow::cycleSortLeft() {
    if(sortDropdown == nullptr || sortDropdown -> count() == 0) {
        return;
    }

    int index = sortDropdown -> currentIndex();

    if(index < sortDropdown -> count() - 1) {
        sortDropdown -> setCurrentIndex(index + 1);
    }
}


/**
 * @brief Cycles the sort dropdown toward the right controller direction.
 */
void MainWindow::cycleSortRight() {
    if(sortDropdown == nullptr || sortDropdown -> count() == 0) {
        return;
    }

    int index = sortDropdown -> currentIndex();

    if(index > 0) {
        sortDropdown -> setCurrentIndex(index - 1);
    }
}


/**
 * @brief Toggles the favourites-only checkbox.
 */
void MainWindow::toggleFavouriteFilter() {
    if(favouriteOnlyBox == nullptr) {
        return;
    }

    favouriteOnlyBox -> setChecked(favouriteOnlyBox -> isChecked() == false);
}


/**
 * @brief Checks if the library screen is the active stacked page.
 *
 * @return `true` when the library is visible.
 */
bool MainWindow::isLibraryVisible() const {
    return appStack != nullptr && appStack -> currentWidget() == libraryWindow;
}


/**
 * @brief Rebuilds the horizontal game shelves.
 */
void MainWindow::rebuildRowShelves() {
    if(shelvesLayout == nullptr) {
        return;
    }

    clearRowShelves();

    if(currentGames.empty() == true) {
        QLabel* emptyLabel = new QLabel("No games in the library.", shelvesContent);
        emptyLabel -> setAlignment(Qt::AlignCenter);
        emptyLabel -> setMinimumSize(220, 140);
        emptyLabel -> setStyleSheet(
            "QLabel {"
            "  border: 1px solid #444444;"
            "  background-color: #1a1a1a;"
            "  color: #dddddd;"
            "  padding: 12px;"
            "  border-radius: 8px;"
            "}"
        );

        shelvesLayout -> addWidget(emptyLabel);
        return;
    }

    std::vector<int> continuePlayingIndexes;
    std::vector<int> favouriteIndexes;

    std::vector<std::string> systemsSeen;
    std::map<std::string, std::vector<int>> systemRows;

    for(int i = 0; i < (int)currentGames.size(); i++) {
        if(currentGames[i].favourite == true) {
            favouriteIndexes.push_back(i);
            continue;
        }

        if(currentGames[i].lastPlayed.empty() == false) {
            continuePlayingIndexes.push_back(i);
            continue;
        }

        const std::string& systemName = currentGames[i].system;
        if(systemRows.find(systemName) == systemRows.end()) {
            systemsSeen.push_back(systemName);
        }

        systemRows[systemName].push_back(i);
            
    }

    if(continuePlayingIndexes.empty() == false) {
        addShelfRow("Continue Playing", continuePlayingIndexes);
    }

    if(favouriteIndexes.empty() == false) {
        addShelfRow("Favourites", favouriteIndexes);
    }

    for(const std::string& systemName : systemsSeen) {
        const std::vector<int>& gameIndexes = systemRows[systemName];

        if(gameIndexes.empty() == false) {
            addShelfRow(QString::fromStdString(systemName), gameIndexes);
        }
    }
}


/**
 * @brief Deletes the current shelf widgets and clears focus indexes.
 */
void MainWindow::clearRowShelves() {
    if(shelvesLayout == nullptr){
        return;
    }

    shelfRows.clear();
    focusedRow = -1;
    focusedColumn = -1;
    preferredColumn = 0;

    while(shelvesLayout -> count() > 0) {
        QLayoutItem* item = shelvesLayout -> takeAt(0);

        if(item == nullptr) continue;

        QWidget* widget = item -> widget();
        if(widget != nullptr) {
            delete widget;
        }

        delete item;
    }
}


/**
 * @brief Adds a horizontal shelf of game cards.
 *
 * @param title Shelf title shown above the cards.
 * @param gameIndexes Indexes of games to include in the shelf.
 */
void MainWindow::addShelfRow(const QString& title, const std::vector<int>& gameIndexes) {
    if(shelvesLayout == nullptr || gameIndexes.empty() == true) {
        return;
    }

    ShelfRow shelfRow;

    QWidget* shelfWidget = new QWidget(shelvesContent);
    QVBoxLayout* rowLayout = new QVBoxLayout(shelfWidget);
    rowLayout -> setContentsMargins(0, 0, 0, 0);
    rowLayout -> setSpacing(6);

    QLabel* rowTitle = new QLabel(title, shelfWidget);
    rowTitle -> setStyleSheet(
        "QLabel {"
        "  color: white;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  padding-left: 1px;"
        "}"
    );

    QWidget* cardRow = new QWidget(shelfWidget);
    QHBoxLayout* cardRowLayout = new QHBoxLayout(cardRow);
    cardRow -> setContentsMargins(0, 0, 0, 0);
    cardRowLayout -> setSpacing(12);

    for(int gameIndex : gameIndexes) {
        if(gameIndex < 0 || gameIndex >= (int)currentGames.size()) {
            continue;
        }

        GameCardWidget* card = new GameCardWidget(cardRow);
        card -> setGame(currentGames[gameIndex]);
        card -> setSelected(gameIndex == selectedIndex);
        card -> setProperty("gameIndex", gameIndex);

        connect(card, &GameCardWidget::clicked, this, [this, gameIndex]() {
            selectGameAtIndex(gameIndex);
            syncFocusToSelectedGame();
            ensureFocusedCardVisible();
        });

        cardRowLayout -> addWidget(card);
        shelfRow.cards.push_back(card);
    }

    cardRowLayout -> addStretch();

    QScrollArea* cardScrollArea = new QScrollArea(shelfWidget);
    cardScrollArea -> setWidgetResizable(true);
    cardScrollArea -> setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    cardScrollArea -> setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    cardScrollArea -> setFrameShape(QFrame::NoFrame);
    cardScrollArea -> setMinimumHeight(195);
    cardScrollArea -> setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QScrollBar:horizontal {"
        "  background: #141414;"
        "  height: 10px;"
        "  margin: 2px 18px 2px 18px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "  background: #3a3a3a;"
        "  min-width: 30px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "  background: #555555;"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "  width: 0px;"
        "  background: none;"
        "  border: none;"
        "}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
        "  background: none;"
        "}"
    );

    cardScrollArea -> setWidget(cardRow);

    rowLayout -> addWidget(rowTitle);
    rowLayout -> addWidget(cardScrollArea);

    shelfRow.shelfWidget = shelfWidget;
    shelfRow.scrollArea = cardScrollArea;
    shelfRow.gameIndexes = gameIndexes;

    shelvesLayout -> addWidget(shelfWidget);
    shelfRows.push_back(shelfRow);
}


/**
 * @brief Applies selected styling to the right game card.
 */
void MainWindow::refreshCardSelection() {
    QList<GameCardWidget*> cards = shelvesContent -> findChildren<GameCardWidget*>();

    for(GameCardWidget* card : cards) {
        bool ok = false;
        int gameIndex = card -> property("gameIndex").toInt(&ok);

        if(ok == true) {
            card -> setSelected(gameIndex == selectedIndex);
        }
        else {
            card -> setSelected(false);
        }
    }
}


/**
 * @brief Selects a game from the current game list.
 *
 * @param index Index of the game to select.
 */
void MainWindow::selectGameAtIndex(int index) {
    if(index < 0 || index >= (int)currentGames.size()) {
        return;
    }

    if(selectedIndex == index) {
        return;
    }

    selectedIndex = index;
    refreshCardSelection();
    emit gameSelectionChanged(currentGames[index].id);
}


/**
 * @brief Sets controller focus to the currently selected game.
 *
 * @return `true` when the selected game was found in a shelf.
 */
bool MainWindow::syncFocusToSelectedGame() {
    if(selectedIndex < 0 || selectedIndex >= (int)currentGames.size()) {
        focusedRow = -1;
        focusedColumn = -1;

        return false;
    }

    for(int rowIndex = 0; rowIndex < (int)shelfRows.size(); rowIndex++) {
        const ShelfRow& row = shelfRows[rowIndex];

        for(int columnIndex = 0; columnIndex < (int)row.gameIndexes.size(); columnIndex++) {
            if(row.gameIndexes[columnIndex] == selectedIndex) {
                focusedRow = rowIndex;
                focusedColumn = columnIndex;
                preferredColumn = columnIndex;

                return true;
            }
        }
    }

    focusedRow = -1;
    focusedColumn = -1;
    return false;
}


/**
 * @brief Checks if the current focus indexes point to an actual card.
 *
 * @return `true` when a focused card exists.
 */
bool MainWindow::hasFocusedCard() const {
    if(focusedRow < 0 || focusedRow >= (int)shelfRows.size()) return false;

    const ShelfRow& row = shelfRows[focusedRow];

    return focusedColumn >= 0
        && focusedColumn < (int)row.gameIndexes.size()
        && focusedColumn < (int)row.cards.size();
}


/**
 * @brief Selects and focuses the first card in the shelves.
 */
void MainWindow::focusFirstAvailableCard() {
    for(int rowIndex = 0; rowIndex < (int)shelfRows.size(); rowIndex++) {
        const ShelfRow& row = shelfRows[rowIndex];

        if(row.gameIndexes.empty() == false) {
            focusedRow = rowIndex;
            focusedColumn = 0;
            preferredColumn = 0;

            selectFocusedGame();
            return;
        }
    }

    focusedRow = -1;
    focusedColumn = -1;
    preferredColumn = 0;
}


/**
 * @brief Selects the game under controller focus.
 */
void MainWindow::selectFocusedGame() {
    if(hasFocusedCard() == false) {
        return;
    }

    const ShelfRow& row = shelfRows[focusedRow];
    int gameIndex = row.gameIndexes[focusedColumn];

    selectGameAtIndex(gameIndex);
}


/**
 * @brief Scrolls the focused card so the user can see it.
 */
void MainWindow::ensureFocusedCardVisible() {
    if(hasFocusedCard() == false) {
        return;
    }

    const ShelfRow& row = shelfRows[focusedRow];
    GameCardWidget* card = row.cards[focusedColumn];

    if(row.scrollArea != nullptr && card != nullptr) {
        int cardLeft = card -> x();
        int cardRight = cardLeft + card -> width();

        QScrollBar* horizontalBar = row.scrollArea -> horizontalScrollBar();
        if(horizontalBar != nullptr) {
            int visibleLeft = horizontalBar -> value();
            int visibleRight = visibleLeft + row.scrollArea -> viewport() -> width();

            if(cardLeft < visibleLeft) {
                horizontalBar -> setValue(cardLeft);
            }
            else if(cardRight > visibleRight) {
                horizontalBar -> setValue(cardRight - row.scrollArea -> viewport() -> width());
            }
        }
    }

    if(shelvesScrollArea != nullptr && row.shelfWidget != nullptr) {
        QScrollBar* verticalBar = shelvesScrollArea -> verticalScrollBar();
        
        if(verticalBar != nullptr) {
            int rowTop = row.shelfWidget -> y();
            int rowBottom = rowTop + row.shelfWidget -> height();
            int visibleTop = verticalBar -> value();
            int visibleBottom = visibleTop + shelvesScrollArea -> viewport() -> height();

            if(rowTop < visibleTop) {
                verticalBar -> setValue(rowTop);
            }
            else if(rowBottom > visibleBottom) {
                verticalBar -> setValue(rowBottom - shelvesScrollArea -> viewport() -> height());
            }
        }
    }
}

/**
 * @file MainWindow.h
 * @brief Declares the main application window that lists games and details.
 */
#pragma once

#include "core/Game.h"
#include "ui/SettingsWindow.h"
#include "library/LibraryService.h"
#include "ui/GameCardWidget.h"

#include <QMainWindow>
#include <vector>
#include <string>
#include <QPointer>

class QFrame;
class QLabel;
class QPushButton;
class QWidget;
class QStackedWidget;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QScrollArea;
class QHBoxLayout;
class QVBoxLayout;
class QMessageBox;


/**
 * @brief Main application window that displays the game library and detail view.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Creates the main application window and its widgets.
     *
     * @param parent Optional parent widget.
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Replaces the visible game list with a new collection.
     *
     * @param games Games to display in the library list.
     */
    void setGames(const std::vector<Game>& newGames);

    /**
     * @brief Returns the identifier for the currently selected game.
     *
     * @return Selected game identifier, or an empty string when nothing is selected.
     */
    std::string getSelectedGameId() const;

    /**
     * @brief Updates the details panel text.
     *
     * @param text Rich or plain text shown in the details area.
     */
    void setDetailsText(const QString& text);

    /**
     * @brief Updates the favourite button label to match the current state.
     *
     * @param favourite Current favourite state of the selected game.
     */
    void setFavouriteButtonText(bool favourite);

    /**
     * @brief Registers the settings screen with the stacked main window.
     *
     * @param settingsWindow Settings widget shown when the user opens settings.
     */
    void setSettingsWindow(SettingsWindow* settingsWindow);

    /**
     * @brief Switches the stacked view to the library screen.
     */
    void showLibrary();

    /**
     * @brief Switches the stacked view to the settings screen.
     */
    void showSettingsWindow();

    /**
     * @brief Displays a modal error dialog.
     *
     * @param message Human-readable error text to show the user.
     */
    void showError(const QString& message);

    /**
     * @brief Returns the current search box contents.
     *
     * @return Raw search text entered by the user.
     */
    std::string getSearchText() const;

    /**
     * @brief Returns the currently selected sort order.
     *
     * @return Sort mode represented by the sort dropdown selection.
     */
    SortMode getSelectedSortOption() const;

    /**
     * @brief Indicates whether the favourites-only filter is enabled.
     *
     * @return `true` when only favourite games should be displayed.
     */
    bool favouriteOnlyEnabled() const;

    /**
     * @brief Moves controller focus one card to the left.
     *
     * @return `true` when focus moved.
     */
    bool moveFocusLeft();

    /**
     * @brief Moves controller focus one card to the right.
     *
     * @return `true` when focus moved.
     */
    bool moveFocusRight();

    /**
     * @brief Moves controller focus to the row above.
     *
     * @return `true` when focus moved.
     */
    bool moveFocusUp();
    
    /**
     * @brief Moves controller focus to the row below.
     *
     * @return `true` when focus moved.
     */
    bool moveFocusDown();

    /**
     * @brief Moves the sort dropdown one option to the left.
     */
    void cycleSortLeft();

    /**
     * @brief Moves the sort dropdown one option to the right.
     */
    void cycleSortRight();

    /**
     * @brief Turns the favourites-only filter on or off.
     */
    void toggleFavouriteFilter();

    /**
     * @brief Checks whether the library page is currently visible.
     *
     * @return `true` when the library screen is active.
     */
    bool isLibraryVisible() const;

signals:
    /**
     * @brief Emitted when the user requests the settings screen.
     */
    void settingsClicked();

    /**
     * @brief Emitted when the user requests to launch the selected game.
     */
    void LaunchClicked();

    /**
     * @brief Emitted when the user requests to toggle the favourite state.
     */
    void favouriteClicked();

    /**
     * @brief Emitted when the game selection changes.
     *
     * @param gameId Identifier of the newly selected game.
     */
    void gameSelectionChanged(const std::string& gameId); 

    /**
     * @brief Emitted when the search text changes.
     */
    void searchChanged();

    /**
     * @brief Emitted when the sort selection changes.
     */
    void sortChanged();

    /**
     * @brief Emitted when the favourites-only checkbox changes.
     */
    void favouriteOnlyChanged();

private:
    /**
     * @brief Holds widgets and game indexes for one horizontal shelf.
     */
    struct ShelfRow {
        QWidget* shelfWidget = nullptr; ///< Widget that owns the shelf row.
        QScrollArea* scrollArea = nullptr; ///< Horizontal scroll area for the cards.
        std::vector<int> gameIndexes; ///< Indexes into the current game list.
        std::vector<GameCardWidget*> cards; ///< Card widgets in this shelf.
    };

    /**
     * @brief Rebuilds all library shelves from the current game list.
     */
    void rebuildRowShelves();

    /**
     * @brief Removes all current shelf widgets and focus state.
     */
    void clearRowShelves();

    /**
     * @brief Adds one shelf row with cards for the given games.
     *
     * @param title Text shown above the shelf.
     * @param gameIndexes Indexes of games to place in the shelf.
     */
    void addShelfRow(const QString& title, const std::vector<int>& gameIndexes);

    /**
     * @brief Updates which cards look selected.
     */
    void refreshCardSelection();

    /**
     * @brief Selects a game by its index in the current list.
     *
     * @param index Index of the game to select.
     */
    void selectGameAtIndex(int index);

    /**
     * @brief Moves controller focus to the selected game card.
     *
     * @return `true` when the selected game was found.
     */
    bool syncFocusToSelectedGame();

    /**
     * @brief Checks whether focus points at a real card.
     *
     * @return `true` when a focused card exists.
     */
    bool hasFocusedCard() const;

    /**
     * @brief Focuses the first card available in the shelves.
     */
    void focusFirstAvailableCard();

    /**
     * @brief Selects the game currently focused by controller navigation.
     */
    void selectFocusedGame();

    /**
     * @brief Scrolls the focused card into view if needed.
     */
    void ensureFocusedCardVisible();
    QStackedWidget* appStack = nullptr;
    QWidget* libraryWindow = nullptr;
    SettingsWindow* settingsWindow = nullptr;

    QWidget* shelvesContent = nullptr;
    QVBoxLayout* shelvesLayout = nullptr;
    QScrollArea* shelvesScrollArea = nullptr;
    QFrame* details = nullptr;
    QPointer<QMessageBox> activeErrorBox;

    QLabel* titleValue = nullptr;
    QLabel* systemValue = nullptr;
    QLabel* favouriteValue = nullptr;
    QLabel* lastPlayedValue = nullptr;
    QLabel* timePlayedValue = nullptr;
    QLabel* romPathValue = nullptr;

    QPushButton* settingsButton = nullptr;
    QPushButton* favouriteButton = nullptr;
    QPushButton* launchButton = nullptr;
    QLineEdit* searchBox = nullptr;
    QComboBox* sortDropdown = nullptr;
    QCheckBox* favouriteOnlyBox = nullptr;

    std::vector<Game> currentGames;
    std::vector<ShelfRow> shelfRows;
    int selectedIndex = -1;
    int focusedRow = -1;
    int focusedColumn = -1;
    int preferredColumn = 0;
};

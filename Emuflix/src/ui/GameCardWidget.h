/**
 * @file GameCardWidget.h
 * @brief Declares the small card used to show one game in the library.
 */
#pragma once

#include "core/Game.h"

#include <QFrame>

class QLabel;
class QMouseEvent;

/**
 * @brief Clickable card that shows a game's title and placeholder art.
 */
class GameCardWidget : public QFrame {
    Q_OBJECT

public: 
    /**
     * @brief Creates an empty game card widget.
     *
     * @param parent Optional parent widget.
     */
    explicit GameCardWidget(QWidget* parent = nullptr);

    /**
     * @brief Sets the game data shown on the card.
     *
     * @param game Game to display.
     */
    void setGame(const Game& game);

    /**
     * @brief Updates whether this card is currently selected.
     *
     * @param selected True when the card should look selected.
     */
    void setSelected(bool selected);

signals:
    /**
     * @brief Emitted when the user clicks the card.
     */
    void clicked();

protected:
    /**
     * @brief Handles mouse clicks and emits the clicked signal.
     *
     * @param event Mouse press event from Qt.
     */
    void mousePressEvent(QMouseEvent* event) override;

private:
    /**
     * @brief Applies the normal or selected card styling.
     */
    void applyCardStyle();

    /**
     * @brief Chooses a placeholder image for a system.
     *
     * @param system Console system name.
     * @return Resource path for the placeholder image.
     */
    QString defaultPathImage(const std::string& system) const;

    Game gameData;
    bool selected = false;

    QLabel* artLabel = nullptr;
    QLabel* titleLabel = nullptr;
};

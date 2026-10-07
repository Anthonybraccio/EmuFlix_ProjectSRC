/**
 * @file GameCardWidget.cpp
 * @brief Implements the clickable game card widget.
 */
#include "ui/GameCardWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QString>

/**
 * @brief Builds the card layout and default styles.
 *
 * @param parent Optional parent widget.
 */
GameCardWidget::GameCardWidget(QWidget* parent) : QFrame(parent) {
    setObjectName("gameCard");
    setCursor(Qt::PointingHandCursor);
    setFixedWidth(165);
    setMinimumHeight(135);
    setMaximumHeight(200);
    setFrameShape(QFrame::NoFrame);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout -> setContentsMargins(0, 0, 0, 0);
    outerLayout -> setSpacing(0);

    artLabel = new QLabel(this);
    artLabel -> setAlignment(Qt::AlignCenter);
    artLabel -> setMinimumHeight(100);
    artLabel->setStyleSheet(
    "QLabel {"
    "  background-color: #2d2d2d;"
    "  color: white;"
    "  border: none;"
    "  border-top-left-radius: 12px;"
    "  border-top-right-radius: 12px;"
    "  font-size: 14px;"
    "  font-weight: bold;"
    "}"
);

    QWidget* info = new QWidget(this);
    QVBoxLayout* infoLayout = new QVBoxLayout(info);
    infoLayout -> setContentsMargins(12, 10, 12, 12);
    infoLayout -> setSpacing(4);

    titleLabel = new QLabel(this);
    titleLabel -> setWordWrap(true);
    titleLabel -> setAlignment(Qt::AlignTop | Qt::AlignLeft);

    infoLayout -> addWidget(titleLabel);
    infoLayout ->addStretch();
    outerLayout -> addWidget(artLabel);
    outerLayout -> addWidget(info);
    
    applyCardStyle();
}


/**
 * @brief Updates the card with a new game's title and image.
 *
 * @param game Game data to display.
 */
void GameCardWidget::setGame(const Game& game) {
    gameData = game;
    titleLabel -> setText(QString::fromStdString(game.title));
    
    QString path = defaultPathImage(game.system);

    QPixmap image(path);
    if(image.isNull() == true) {
        image.load(":/images/placeholders/generic.png");
    }

    

    artLabel -> setText("");
    artLabel -> setPixmap(image.scaled(165, 135, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    applyCardStyle();

}


/**
 * @brief Sets whether the card is selected.
 *
 * @param isSelected New selected state.
 */
void GameCardWidget::setSelected(bool isSelected) {
    selected = isSelected;
    applyCardStyle();
}


/**
 * @brief Emits the card click signal when the mouse is pressed.
 *
 * @param event Mouse event passed in by Qt.
 */
void GameCardWidget::mousePressEvent(QMouseEvent* event) {
    emit clicked();
    QFrame::mousePressEvent(event);
}


/**
 * @brief Updates stylesheet values based on selected state.
 */
void GameCardWidget::applyCardStyle() {
    if(selected == true) {
        setStyleSheet(
            "#gameCard {"
            "  background-color: #242424;"
            "  border: 3px solid #e50914;"
            "  border-radius: 12px;"
            "}"
        );

        titleLabel->setStyleSheet(
            "QLabel {"
            "  color: white;"
            "  font-size: 14px;"
            "  font-weight: bold;"
            "  border: none;"
            "}"
        );
    }
    else {
        setStyleSheet(
            "#gameCard {"
            "  background-color: #151515;"
            "  border: 1px solid #3f3f3f;"
            "  border-radius: 12px;"
            "}"
            "#gameCard:hover {"
            "  border: 2px solid #777777;"
            "  background-color: #1e1e1e;"
            "}"
        );

        titleLabel->setStyleSheet(
            "QLabel {"
            "  color: #f0f0f0;"
            "  font-size: 14px;"
            "  font-weight: bold;"
            "  border: none;"
            "}"
        );
    }
}


/**
 * @brief Finds the placeholder image for a console system.
 *
 * @param system Console system name.
 * @return Qt resource path for the placeholder image.
 */
QString GameCardWidget::defaultPathImage(const std::string& system) const {
    QString systemText = QString::fromStdString(system).trimmed().toLower();

    if(systemText == "nes") {
        return ":/images/placeholders/nes.png";
    }

    if(systemText == "super nes") {
        return ":/images/placeholders/snes.png";
    }

    if(systemText == "game boy advance") {
        return ":/images/placeholders/gba.png";
    }

    if(systemText == "game boy") {
        return ":/images/placeholders/gb.png";
    }

    if(systemText == "game boy color") {
        return ":/images/placeholders/gbc.png";
    }

    if(systemText == "nintendo 64") {
        return ":/images/placeholders/n64.png";
    }

    return ":/images/placeholders/generic.png";
}

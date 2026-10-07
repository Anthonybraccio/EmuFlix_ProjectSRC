/**
 * @file ControllerService.h
 * @brief Declares controller input polling for the UI.
 */
#pragma once

struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;

/**
 * @brief One-frame controller actions used by the UI.
 */
struct Actions {
    bool moveLeft = false;  ///< User pressed left this frame.
    bool moveRight = false; ///< User pressed right this frame.
    bool moveUp = false;    ///< User pressed up this frame.
    bool moveDown = false;  ///< User pressed down this frame.
    
    bool launch = false;        ///< User requested launching a game.
    bool toggleFav = false;     ///< User requested toggling favourite.
    bool toggleFavOnly = false; ///< User requested the favourites-only filter.
    bool requestExit = false;   ///< User requested going back or exiting.
    bool openSettings = false;  ///< User requested opening settings.

    bool sortLeft = false;  ///< User requested the previous sort option.
    bool sortRight = false; ///< User requested the next sort option.
};

/**
 * @brief Wraps SDL controller setup and turns button presses into UI actions.
 */
class ControllerService {

public:
    /**
     * @brief Creates the controller service.
     */
    ControllerService();

    /**
     * @brief Shuts down controller input when the service is destroyed.
     */
    ~ControllerService();

    /**
     * @brief Starts SDL controller support and opens the first controller.
     *
     * @return `true` when controller support starts successfully.
     */
    bool initialize();

    /**
     * @brief Stops controller support and closes any open controller.
     */
    void shutdown();

    /**
     * @brief Polls the controller and returns newly pressed actions.
     *
     * @return Actions pressed since the previous poll.
     */
    Actions poll();

private:
    /**
     * @brief Opens the first available SDL game controller.
     *
     * @return `true` when a controller was opened.
     */
    bool firstAvailableController();

    /**
     * @brief Closes the current controller if one is open.
     */
    void closeController();

    bool initialized = false;
    SDL_GameController* controller = nullptr;

    bool previousLeft = false;
    bool previousRight = false;
    bool previousUp = false;
    bool previousDown = false;
    
    bool previousLaunch = false;
    bool previousToggleFav = false;
    bool previousToggleFavOnly = false;
    bool previousRequestExit = false;

    bool previousSortLeft = false;
    bool previousSortRight = false;
    bool previousOpenSettings = false;
};

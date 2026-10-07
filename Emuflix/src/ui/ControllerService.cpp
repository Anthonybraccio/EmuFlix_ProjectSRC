/**
 * @file ControllerService.cpp
 * @brief Implements SDL controller polling for the UI.
 */
#include "ui/ControllerService.h"

#include <SDL.h>

/**
 * @brief Creates the service with no controller opened yet.
 */
ControllerService::ControllerService() {
}


/**
 * @brief Makes sure SDL controller support is shut down.
 */
ControllerService::~ControllerService() {
    shutdown();
}


/**
 * @brief Starts SDL controller input and opens the first controller found.
 *
 * @return `true` if SDL started and a controller is ready.
 */
bool ControllerService::initialize() {
    if(initialized == true) {
        return true;
    }

    if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        return false;
    }

    initialized = true;
    return firstAvailableController();
}


/**
 * @brief Closes the controller and stops SDL controller input.
 */
void ControllerService::shutdown() {
    if(initialized == false) {
        return;
    }

    closeController();
    
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    initialized = false;
} 


/**
 * @brief Reads controller buttons and returns actions for new presses.
 *
 * @return Actions that happened during this poll.
 */
Actions ControllerService::poll() {
    Actions actions;

    if(initialized == false) {
        return actions;
    }

    SDL_GameControllerUpdate();

    if(controller == nullptr || SDL_GameControllerGetAttached(controller) == SDL_FALSE) {
        firstAvailableController();
    }

    if(controller == nullptr) {
        previousLeft = false;
        previousRight = false;
        previousUp = false;
        previousDown = false;

        previousLaunch = false;
        previousToggleFav = false;
        previousToggleFavOnly = false;
        previousRequestExit = false;

        previousSortLeft = false;
        previousSortRight = false;
        previousOpenSettings = false;

        return actions;
    }

    bool dpadLeft = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) != 0;
    bool dpadRight = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) != 0;
    bool dpadUp = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) != 0;
    bool dpadDown = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) != 0;
    
    const Sint16 axisDeadzone = 16000;
    Sint16 axisX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    Sint16 axisY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

    bool stickLeft = axisX < -axisDeadzone;
    bool stickRight = axisX > axisDeadzone;
    bool stickUp = axisY < -axisDeadzone;
    bool stickDown = axisY > axisDeadzone;

    bool left = dpadLeft || stickLeft;
    bool right = dpadRight || stickRight;
    bool up = dpadUp || stickUp;
    bool down = dpadDown || stickDown;

    bool launch = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A) != 0;
    bool toggleFavourite = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y) != 0;
    bool toggleFavouriteOnly = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X) != 0;
    bool requestExit = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B) != 0;

    bool sortLeft = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) != 0;
    bool sortRight = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) != 0;
    bool openSettings = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START) != 0;

    actions.moveLeft = (left == true && previousLeft == false);
    actions.moveRight = (right == true && previousRight == false);
    actions.moveUp = (up == true && previousUp == false);
    actions.moveDown = (down == true && previousDown == false);

    actions.launch = (launch == true && previousLaunch == false);
    actions.toggleFav = (toggleFavourite == true && previousToggleFav == false);
    actions.toggleFavOnly = (toggleFavouriteOnly == true && previousToggleFavOnly == false);
    actions.requestExit = (requestExit == true && previousRequestExit == false);

    actions.sortLeft = (sortLeft == true && previousSortLeft == false);
    actions.sortRight = (sortRight == true && previousSortRight == false);
    actions.openSettings = (openSettings == true && previousOpenSettings == false);

    previousLeft = left;
    previousRight = right;
    previousUp = up;
    previousDown = down;

    previousLaunch = launch;
    previousToggleFav = toggleFavourite;
    previousToggleFavOnly = toggleFavouriteOnly;
    previousRequestExit = requestExit;

    previousSortLeft = sortLeft;
    previousSortRight = sortRight;
    previousOpenSettings = openSettings;

    return actions;
}


/**
 * @brief Finds and opens the first SDL game controller.
 *
 * @return `true` when a controller was opened.
 */
bool ControllerService::firstAvailableController() {
    closeController();
    int joySticks = SDL_NumJoysticks();

    for(int i = 0; i < joySticks; i++) {
        if(SDL_IsGameController(i) == SDL_FALSE) continue;

        controller = SDL_GameControllerOpen(i);
        if(controller != nullptr) {
            return true;
        }
    }

    return false;
}


/**
 * @brief Closes the active SDL controller.
 */
void ControllerService::closeController() {
    if(controller != nullptr) {
        SDL_GameControllerClose(controller);

        controller = nullptr;
    }
}

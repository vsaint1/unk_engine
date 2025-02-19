#include "../public/core/Engine.h"


SDL_AppResult Engine::Initialize(const char* title, glm::vec2 window_size, SDL_WindowFlags flags) {

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to Initialize %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto _window = SDL_CreateWindow(title, window_size.x, window_size.y, flags);

    if (!_window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    this->window = _window;
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Engine initialized successfully \n");
    return SDL_APP_SUCCESS;
}

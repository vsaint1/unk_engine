#pragma once
#include "helpers/Logging.h"

class Engine {

    glm::vec2 windowSize;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioDeviceID audioDevice;
    const SDL_DisplayMode* displayMode;
    
public:
    SDL_AppResult engineState = SDL_APP_CONTINUE;

    Engine() {}
    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;

    SDL_AppResult Initialize(const char* title = "[UNK_ENGINE] - Window", glm::vec2 window_size = {800, 600},
                             SDL_WindowFlags flags = SDL_WINDOW_OPENGL, bool bEnableVsync = true);

    SDL_AppResult CreateRenderer();

    SDL_AppResult InitializeModules();

    SDL_Window* GetWindow() {
        return this->window;
    }

    SDL_Renderer* GetRenderer() {
        return this->renderer;
    }

    SDL_AudioDeviceID GetAudioDevice() {
        return this->audioDevice;
    }
};

inline auto GEngine = std::make_unique<Engine>();

#pragma once
#include "Macros.h"

class Engine {

    glm::vec2 windowSize;
    SDL_Window* window;

public:
    Engine() {}
    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;

    SDL_AppResult Initialize(const char* title = "[UNK_ENGINE] - Window", glm::vec2 window_size = {800, 600}, SDL_WindowFlags flags = SDL_WINDOW_OPENGL);

    SDL_Window* GetWindow() {
        return this->window;
    }
};

inline auto GEngine = std::make_unique<Engine>();

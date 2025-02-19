#include "../public/core/Engine.h"


SDL_AppResult Engine::Initialize(const char* title, glm::vec2 window_size, SDL_WindowFlags flags, bool bEnableVsync) {

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
        LOG_CRITICAL("Failed to Initialize SDL %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto _window = SDL_CreateWindow(title, window_size.x, window_size.y, flags);

    if (!_window) {
        LOG_CRITICAL("Failed to create window %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    this->window = _window;

    int displayIndex = SDL_GetDisplayForWindow(GEngine->GetWindow());

    this->displayMode = SDL_GetCurrentDisplayMode(displayIndex);

    LOG_INFO("DisplayID[%d], Width %d, Height %d, Refresh rate %f \n", displayMode->displayID, displayMode->w,
             displayMode->h, displayMode->refresh_rate);

    LOG_INFO("Engine initialized successfully \n");

    if (this->CreateRenderer() == SDL_APP_FAILURE) {
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderVSync(this->renderer, bEnableVsync ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED);

    if (this->InitializeModules() == SDL_APP_FAILURE) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_SUCCESS;
}

SDL_AppResult Engine::CreateRenderer() {

    SDL_Renderer* _renderer = SDL_CreateRenderer(window, NULL);

    if (!_renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    this->renderer = _renderer;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Engine renderer created successfully \n");

    return SDL_APP_SUCCESS;
}

SDL_AppResult Engine::InitializeModules() {

    auto audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 0);

    if (!audioDevice) {

        LOG_ERROR("Failed to open audio device %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }


    if (!Mix_OpenAudio(audioDevice, NULL)) {
        LOG_ERROR("Failed to initialize Mixer %s \n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {

        LOG_ERROR("Failed to initialize TTF %s \n", SDL_GetError());

        return SDL_APP_FAILURE;
    }


    return SDL_APP_SUCCESS;
}

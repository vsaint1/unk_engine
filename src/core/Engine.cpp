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

#if GLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(_WIN32)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GLContext glContext = SDL_GL_CreateContext(GEngine->GetWindow());

    if (glContext) {
        auto vendorName = glGetString(GL_VENDOR);
        LOG_INFO("Vendor %s \n", vendorName);
    }

#endif


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

void Engine::Cleanup() {

    SDL_DestroyRenderer(GEngine->GetRenderer());
    SDL_DestroyWindow(GEngine->GetWindow());

    Mix_FadeOutMusic(1000);
    Mix_CloseAudio();
    SDL_CloseAudioDevice(GEngine->GetAudioDevice());

    TTF_Quit();
    Mix_Quit();

    LOG_INFO("Application cleaned and quitting successfully \n");

    SDL_Quit();
}

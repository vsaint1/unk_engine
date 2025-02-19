#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_system.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>


#if __ANDROID__
const std::filesystem::path BASE_PATH = "";
#define ASSETS_PATH std::string("")
#else
const std::filesystem::path BASE_PATH = SDL_GetBasePath();
#define ASSETS_PATH std::string("assets/")
#endif


struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* messageTex;
    SDL_Texture* imageTex;
    SDL_FRect messageDest;
    SDL_FRect imageDest;
    SDL_AudioDeviceID audioDevice;
    Mix_Music* music;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};


std::string LoadFile(const std::string& filename) {

    size_t filesize     = 0;
    const auto filePath = ASSETS_PATH.append(filename);


    void* data = SDL_LoadFile(filePath.c_str(), &filesize);

    if (!data) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load file '%s': %s \n", filename.c_str(), SDL_GetError());
        return "";
    }

    std::string content(static_cast<char*>(data), filesize);
    SDL_free(data);

    return content;
}

SDL_AppResult SDL_Fail() {
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s \n", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_Fail();
    }


    if (!TTF_Init()) {
        return SDL_Fail();
    }


    SDL_Window* window = SDL_CreateWindow("[UNK_ENGINE] - Window", 800, 600,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL);
    if (!window) {
        return SDL_Fail();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer) {
        return SDL_Fail();
    }

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "path %s \n", SDL_GetBasePath());

    auto textFile = LoadFile("test.txt");

    TTF_Font* mineFont = TTF_OpenFont(ASSETS_PATH.append("fonts/Minecraft.ttf").c_str(), 32);

    if (!mineFont) {
        return SDL_Fail();
    }

    auto aduioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 0);
    if (!aduioDevice) {
        return SDL_Fail();
    }

    if (!Mix_OpenAudio(aduioDevice, 0)) {
        return SDL_Fail();
    }

    auto music = Mix_LoadMUS(ASSETS_PATH.append("sounds/lullaby.mp3").c_str());

    if (!music) {
        return SDL_Fail();
    }

    Mix_PlayMusic(music, 0); // once

    SDL_Surface* imgSurface = IMG_Load(ASSETS_PATH.append("images/dragon.png").c_str());

    SDL_Texture* imageTex = SDL_CreateTextureFromSurface(renderer, imgSurface);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(mineFont, textFile.data(), textFile.length(), {255, 255, 255});

    SDL_Texture* messageTex = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Content %s\n", textFile.c_str());

    TTF_CloseFont(mineFont);

    SDL_DestroySurface(surfaceMessage);
    SDL_DestroySurface(imgSurface);

    SDL_ShowWindow(window);
    {
        SDL_Log("Working flawlessly in %s \n", SDL_GetPlatform());

        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i \n", width, height);
        SDL_Log("Backbuffer size: %ix%i \n", bbwidth, bbheight);
        if (width != bbwidth) {
            SDL_Log("This is a highdpi environment. \n");
        }
    }


    SDL_FRect messageRect = {10, 100, messageTex->w, messageTex->h};

    SDL_FRect imageRect = {50,200, 256, 256};

    *appstate = new AppContext{.window      = window,
                               .renderer    = renderer,
                               .messageTex  = messageTex,
                               .imageTex    = imageTex,
                               .messageDest = messageRect,
                               .imageDest   = imageRect,
                               .music       = music};

    SDL_SetRenderVSync(renderer, -1); // enable vysnc


    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    auto* app = (AppContext*) appstate;

    if (event->type == SDL_EVENT_QUIT) {
        app->app_quit = SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = (AppContext*) appstate;

    auto deltaTime = SDL_GetTicks() / 1000.f;
    const auto FREQUENCY = 10;
    float lerpFactor = (std::sin(deltaTime * FREQUENCY) + 1) / 2.0;

    auto red   = 200 * (1 - lerpFactor) + 255 * lerpFactor;
    auto green = 0 * (1 - lerpFactor) + 255 * lerpFactor;
    auto blue  = 0 * (1 - lerpFactor) + 255 * lerpFactor;

    SDL_SetRenderDrawColor(app->renderer, 0,0,0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(app->renderer);

    SDL_SetTextureColorMod(app->imageTex,red,green,blue); // just a small example of color modulation
    SDL_RenderTexture(app->renderer, app->imageTex, NULL, &app->imageDest);

    SDL_RenderTexture(app->renderer, app->messageTex, NULL, &app->messageDest);

    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*) appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);

        Mix_FadeOutMusic(1000);
        Mix_FreeMusic(app->music);
        Mix_CloseAudio();
        SDL_CloseAudioDevice(app->audioDevice);

        delete app;
    }

    TTF_Quit();
    Mix_Quit();
    SDL_Log("Application cleaned and quitting successfully \n");

    SDL_Quit();
}

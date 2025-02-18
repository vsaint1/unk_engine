#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
// #include <SDL3_ttf/SDL_ttf.h>
// #include <SDL3_mixer/SDL_mixer.h>
// #include <SDL3_image/SDL_image.h>
#include <cmath>
#include <filesystem>
#include <string_view>

constexpr uint32_t windowStartWidth  = 400;
constexpr uint32_t windowStartHeight = 400;

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture *messageTex, *imageTex;
    SDL_FRect messageDest;
    SDL_AudioDeviceID audioDevice;
    // Mix_Music* music;
    SDL_AppResult app_quit = SDL_APP_CONTINUE;
};

SDL_AppResult SDL_Fail() {
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        return SDL_Fail();
    }


    SDL_Window* window = SDL_CreateWindow("SDL3 hello", windowStartWidth, windowStartHeight,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (!window) {
        return SDL_Fail();
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        return SDL_Fail();
    }

#if __ANDROID__
    std::filesystem::path basePath = "";
#else
    auto basePathPtr = SDL_GetBasePath();
    if (not basePathPtr) {
        return SDL_Fail();
    }
    const std::filesystem::path basePath = basePathPtr;
#endif


    SDL_ShowWindow(window);
    {
        SDL_Log("Working flawlessly in %s", SDL_GetPlatform());

        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(window, &width, &height);
        SDL_GetWindowSizeInPixels(window, &bbwidth, &bbheight);
        SDL_Log("Window size: %ix%i", width, height);
        SDL_Log("Backbuffer size: %ix%i", bbwidth, bbheight);
        if (width != bbwidth) {
            SDL_Log("This is a highdpi environment.");
        }
    }

    *appstate = new AppContext{
        .window   = window,
        .renderer = renderer,
    };

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

    auto time  = SDL_GetTicks() / 1000.f;
    auto red   = (std::sin(time) + 1) / 2.0 * 255;
    auto green = (std::sin(time / 2) + 1) / 2.0 * 255;
    auto blue  = (std::sin(time) * 2 + 1) / 2.0 * 255;

    SDL_SetRenderDrawColor(app->renderer, red, green, blue, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);


    SDL_RenderPresent(app->renderer);

    return app->app_quit;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*) appstate;
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        delete app;
    }

    SDL_Quit();
}

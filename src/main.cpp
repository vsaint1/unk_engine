#include "public/core/Engine.h"
#include <SDL3/SDL_main.h> /* WARNING: This must be included only once! */

struct AppContext {
    SDL_Texture* messageTex;
    SDL_Texture* imageTex;
    SDL_FRect messageDest;
    SDL_FRect imageDest;
    Mix_Music* music;
};


std::string LoadFile(const std::string& filename) {

    size_t filesize     = 0;
    const auto filePath = ASSETS_PATH.append(filename);


    void* data = SDL_LoadFile(filePath.c_str(), &filesize);

    if (!data) {
        LOG_ERROR("Failed to load file '%s': %s \n", filename.c_str(), SDL_GetError());
        return "";
    }

    std::string content(static_cast<char*>(data), filesize);
    SDL_free(data);

    return content;
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {


    /* This will fail and not continue if SDL_Init/CreateWindow fails */
    GEngine->Initialize("[UNK_ENGINE] - Window", {800, 600},
                       SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL, true);


#if GLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#endif

    SDL_GLContext glContext = SDL_GL_CreateContext(GEngine->GetWindow());

    LOG_QUIT_ON_FAIL(glContext);

    auto vendorName = glGetString(GL_VENDOR);
    auto glRenderer = glGetString(GL_RENDERER);

    LOG_INFO("Vendor %s, Renderer %s\n", vendorName, glRenderer);

    auto textFile = LoadFile("test.txt");
    auto jsonFile = LoadFile("test.json");

    Json json = Json::parse(jsonFile);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "JSON: %s \n", json.dump(4).c_str());

    TTF_Font* mineFont = TTF_OpenFont(ASSETS_PATH.append("fonts/Minecraft.ttf").c_str(), 32);

    LOG_QUIT_ON_FAIL(mineFont);

    auto music = Mix_LoadMUS(ASSETS_PATH.append("sounds/lullaby.mp3").c_str());

    LOG_QUIT_ON_FAIL(music);

    auto vec = glm::vec3(1, 1, 1);

    LOG_INFO("glm_vec %f %f %f \n", vec.x, vec.y, vec.z);

    Mix_PlayMusic(music, 0); // once

    SDL_Surface* imgSurface = IMG_Load(ASSETS_PATH.append("images/dragon.png").c_str());

    SDL_Texture* imageTex = SDL_CreateTextureFromSurface(GEngine->GetRenderer(), imgSurface);

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(mineFont, textFile.data(), textFile.length(), {255, 255, 255});

    SDL_Texture* messageTex = SDL_CreateTextureFromSurface(GEngine->GetRenderer(), surfaceMessage);

    LOG_INFO("Content %s\n", textFile.c_str());

    TTF_CloseFont(mineFont);

    SDL_DestroySurface(surfaceMessage);
    SDL_DestroySurface(imgSurface);

    auto gpuDriver = SDL_GetGPUDriver(0);
    SDL_ShowWindow(GEngine->GetWindow());
    {
        LOG_INFO("Working flawlessly in %s \n", SDL_GetPlatform());
        LOG_INFO("Version %d \n", SDL_GetVersion());
        LOG_INFO("Video %s \n", SDL_GetVideoDriver(0));
        LOG_INFO("GPU driver %s \n", SDL_GetGPUDriver(0));
        LOG_INFO("RAM available %d \n", SDL_GetSystemRAM());

        int width, height, bbwidth, bbheight;
        SDL_GetWindowSize(GEngine->GetWindow(), &width, &height);
        SDL_GetWindowSizeInPixels(GEngine->GetWindow(), &bbwidth, &bbheight);
        LOG_INFO("Window size: %ix%i \n", width, height);
        LOG_INFO("Backbuffer size: %ix%i \n", bbwidth, bbheight);
        if (width != bbwidth) {
            LOG_INFO("This is a highdpi environment. \n");
        }
    }


    SDL_FRect messageRect = {10, 100, messageTex->w, messageTex->h};

    SDL_FRect imageRect = {50, 200, 256, 256};

    *appstate = new AppContext{

        .messageTex  = messageTex,
        .imageTex    = imageTex,
        .messageDest = messageRect,
        .imageDest   = imageRect,
        .music       = music};


    return GEngine->engineState;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    auto* app = (AppContext*) appstate;

    if (event->type == SDL_EVENT_QUIT) {
        GEngine->engineState = SDL_APP_SUCCESS;
    }

    return GEngine->engineState;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = (AppContext*) appstate;

    auto deltaTime       = SDL_GetTicks() / 1000.f;
    const auto FREQUENCY = 5;
    float lerpFactor     = std::sin(deltaTime * FREQUENCY) / 2.0;

    auto red   = 200 * (1 - lerpFactor) + 255 * lerpFactor;
    auto green = 255 * (1 - lerpFactor) + 255 * lerpFactor;
    auto blue  = 255 * (1 - lerpFactor) + 255 * lerpFactor;

    SDL_SetRenderDrawColor(GEngine->GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(GEngine->GetRenderer());

    SDL_SetTextureColorMod(app->imageTex, red, green, blue); // just a small example of color modulation
    SDL_RenderTexture(GEngine->GetRenderer(), app->imageTex, NULL, &app->imageDest);

    SDL_RenderTexture(GEngine->GetRenderer(), app->messageTex, NULL, &app->messageDest);

    SDL_RenderPresent(GEngine->GetRenderer());

    return GEngine->engineState;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (AppContext*) appstate;
    if (app) {
        SDL_DestroyRenderer(GEngine->GetRenderer());
        SDL_DestroyWindow(GEngine->GetWindow());

        Mix_FadeOutMusic(1000);
        Mix_FreeMusic(app->music);
        Mix_CloseAudio();
        SDL_CloseAudioDevice(GEngine->GetAudioDevice());

        delete app;
    }

    TTF_Quit();
    Mix_Quit();
    LOG_INFO("Application cleaned and quitting successfully \n");

    SDL_Quit();
}

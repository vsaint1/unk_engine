#include "public/core/Engine.h"
#include "public/core/TileMgr.h"
#include "public/core/Timer.h"
#include <SDL3/SDL_main.h> /* WARNING: This must be included only once! */


/* JUST FOR DEVELOPMENT AND EXAMPLE */
struct GameContext {
    SDL_Texture* messageTex;
    SDL_Texture* imageTex;
    SDL_FRect messageDest;
    SDL_FRect imageDest;
    Mix_Music* music;
    TiledMap map;
};


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {


    /* This will fail and not continue if SDL_Init/CreateWindow fails */
    GEngine->Initialize("[UNK_ENGINE] - Window", {800, 600},
                        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL,
                        false);

    auto textFile = ResourceManager::GetInstance().LoadFromFile("test.txt");
    auto jsonFile = ResourceManager::GetInstance().LoadFromFile("test.json");

    TiledMap map("map/overworld_map.xml");

    map.Awake();

    Json json      = Json::parse(jsonFile);
    json["player"] = "John Doe";
    json["health"] = 100;

    // Small example saving data
    ResourceManager::GetInstance().WriteToFile("save.dat", json.dump().c_str(), json.dump().length());

    LOG_INFO("JSON: %s \n", json.dump(4).c_str());

    auto mineFont = ResourceManager::GetInstance().GetFont("fonts/Minecraft.ttf", 32);

    LOG_QUIT_ON_FAIL(mineFont);

    auto music = ResourceManager::GetInstance().GetMusic("sounds/lullaby.mp3"); // Auto-cleanup

    LOG_QUIT_ON_FAIL(music);

    auto vec = glm::vec3(1, 1, 1);

    LOG_INFO("glm_vec %f %f %f \n", vec.x, vec.y, vec.z);

    Mix_PlayMusic(music, 0); // once


    SDL_Texture* image = ResourceManager::GetInstance().GetTexture("images/dragon.png");

    SDL_Texture* message =
        ResourceManager::GetInstance().CreateFontTexture("message", textFile, mineFont, {255, 255, 255});

    SDL_Texture* hello =
        ResourceManager::GetInstance().CreateFontTexture("intro_message", "Hello world", mineFont, {255, 255, 255});

    LOG_INFO("Content %s\n", textFile.c_str());

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


    SDL_FRect messageRect = {10, 100, message->w, message->h};

    SDL_FRect imageRect = {50, 200, 256, 256};

    *appstate = new GameContext{

        .messageTex = message, .imageTex = image, .messageDest = messageRect, .imageDest = imageRect, .map = map

    };


    return GEngine->engineState;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    auto* app = (GameContext*) appstate;

    if (event->type == SDL_EVENT_QUIT) {
        GEngine->engineState = SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.scancode == SDL_SCANCODE_P) {

        if (GFrameManager->IsPaused()) {
            LOG_INFO("Resuming \n");
            GFrameManager->Resume();
        } else {
            LOG_INFO("Pausing \n");

            GFrameManager->Pause();
        }
    }

    return GEngine->engineState;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = (GameContext*) appstate;


    GFrameManager->Update();

    const auto gameStatus = GFrameManager->IsPaused() ? "PAUSED" : "RUNNING";
    std::string title     = "[UNK_ENGINE] - Window - FPS: " + GFrameManager->GetFpsText() + gameStatus;

    char titleFmt[0x100];
    SDL_snprintf(titleFmt, sizeof(titleFmt), "[%s:%s] - Window Sample, [FPS]: %.2f, [GameState]: %s ", ENGINE_NAME,
                 ENGINE_VERSION_STR, GFrameManager->GetFps(), gameStatus);

    SDL_SetWindowTitle(GEngine->GetWindow(), titleFmt);

    const float FREQUENCY = 2.f;
    float lerpFactor      = (std::sin(GFrameManager->GetElapsedTime() * FREQUENCY * M_PI) + 1.0f) * 0.5f;

    auto red   = 150 * (1 - lerpFactor) + 255 * lerpFactor;
    auto green = 255 * (1 - lerpFactor) + 255 * lerpFactor;
    auto blue  = 255 * (1 - lerpFactor) + 255 * lerpFactor;

    SDL_SetRenderDrawColor(GEngine->GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(GEngine->GetRenderer());

    SDL_SetTextureColorMod(app->imageTex, red, green, blue);
    SDL_SetTextureScaleMode(app->imageTex, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);

    // WARNING: order matters!!!

    app->map.Render(GEngine->GetRenderer());

    SDL_RenderTexture(GEngine->GetRenderer(), app->messageTex, NULL, &app->messageDest);
    SDL_RenderTexture(GEngine->GetRenderer(), app->imageTex, NULL, &app->imageDest);

    SDL_RenderPresent(GEngine->GetRenderer());

    GFrameManager->FixedFrameRate(1);

    return GEngine->engineState;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto* app = (GameContext*) appstate;
    if (app) {
        delete app;
    }

    GEngine->Cleanup();
}

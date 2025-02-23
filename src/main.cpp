#include "public/core/Engine.h"
#include "public/core/Timer.h"
#include "public/core/components/Camera.h"
#include "public/core/components/Sprite.h"
#include "public/core/components/TileMgr.h"
#include "public/core/components/Transform.h"
#include <SDL3/SDL_main.h> /* WARNING: This must be included only once! */

/* JUST FOR DEVELOPMENT AND EXAMPLE */
struct GameContext {
    SDL_Texture* messageTex;
    SDL_FRect messageDest;
    Mix_Music* music;
    flecs::world world;
};

enum EEntityType { NONE = 0, PLAYER, CREATURE, ENEMY };

// TODO: refactor
struct Entity {
    std::string name = "unk";
    float health     = 0;
    float maxHealth  = 0;
    EEntityType type = NONE;
};

// TODO: refactor
void HandlePlayerInput(Transform* playerTransform, float speed, float deltaTime) {
    auto state = SDL_GetKeyboardState(NULL);

    auto pos = playerTransform->GetPosition();

    if (state[SDL_SCANCODE_W]) {
        pos.y -= speed * deltaTime;
    }
    if (state[SDL_SCANCODE_S]) {
        pos.y += speed * deltaTime;
    }
    if (state[SDL_SCANCODE_A]) {
        pos.x -= speed * deltaTime;
    }
    if (state[SDL_SCANCODE_D]) {
        pos.x += speed * deltaTime;
    }

    playerTransform->SetPosition({pos.x, pos.y, 0.0f});
}

// TODO: refactor
void RenderHealthBar(SDL_Renderer* renderer, Transform* transform, Entity* entity) {
    const int barWidth  = 32;
    const int barHeight = 4;
    int healthWidth     = static_cast<int>((entity->health / entity->maxHealth) * barWidth);

    SDL_FRect background = {transform->GetPosition().x, transform->GetPosition().y - 2, barWidth, barHeight};
    SDL_FRect healthBar  = {transform->GetPosition().x, transform->GetPosition().y - 2, (float) healthWidth, barHeight};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, &background);

    SDL_SetRenderDrawColor(renderer, 0, 150, 0, 200);
    SDL_RenderFillRect(renderer, &healthBar);
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {


    /* This will fail and not continue if SDL_Init/CreateWindow fails */
    GEngine->Initialize("[UNK_ENGINE] - Window", {1280, 720}, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_OPENGL, false);


    int width, height, bbwidth, bbheight;
    SDL_GetWindowSize(GEngine->GetWindow(), &width, &height);
    SDL_GetWindowSizeInPixels(GEngine->GetWindow(), &bbwidth, &bbheight);

    SDL_ShowWindow(GEngine->GetWindow());

    auto textFile = ResourceManager::GetInstance().LoadFromFile("test.txt");
    auto jsonFile = ResourceManager::GetInstance().LoadFromFile("test.json");

    SDL_Texture* image = ResourceManager::GetInstance().GetTexture("images/dragon.png");

    SDL_Texture* player = ResourceManager::GetInstance().GetTexture("images/player.png");

    flecs::world GWorld;
    
    LOG_INFO("initialized flecs");
    flecs::entity starter_scene = GWorld.entity("start_scene")
                                      .set<TiledMap>({"map/overworld_map_test_obj.xml"});

    flecs::entity dragon = GWorld.entity("enemy_dragon")
                               .set<Transform>({{10, 20, 0}, {0, 0, 0}, {1, 1}})
                               .set<Sprite>({image, {32, 32}, {0, 0}, {32, 32}})
                               .set<Entity>({"Valakas", 100, 1000, EEntityType::ENEMY})
                               .child_of(starter_scene);

    GWorld.entity("player")
        .set<Transform>({{40, 50, 0}, {0, 0, 0}, {1, 1}})
        .set<Sprite>({player, {16, 16}, {10, 10}, {16, 16}})
        .set<Entity>({"Player", 100, 100, EEntityType::PLAYER})
        .child_of(starter_scene);

    GWorld.each([&](flecs::entity e, TiledMap& map) {
        map.Awake();

        map.Start();

    });


    Json json      = Json::parse(jsonFile);
    json["player"] = "John Doe";
    json["health"] = 100;

    // Small example saving data
    ResourceManager::GetInstance().WriteToFile("save.dat", json.dump().c_str(), json.dump().length());

    LOG_INFO("JSON: %s \n", json.dump(4).c_str());


    auto mineFont = ResourceManager::GetInstance().GetFont("fonts/Minecraft.ttf", 32);

    LOG_QUIT_ON_FAIL(mineFont);

    auto music = ResourceManager::GetInstance().GetMusic("sounds/the_entertainer.ogg"); // Auto-cleanup

    LOG_QUIT_ON_FAIL(music);

    Mix_PlayMusic(music, 0); // once
    auto vec = glm::vec3(1, 1, 1);

    LOG_INFO("glm_vec %f %f %f \n", vec.x, vec.y, vec.z);


    SDL_Texture* message =
        ResourceManager::GetInstance().CreateFontTexture("message", textFile, mineFont, {255, 255, 255});

    SDL_Texture* hello = ResourceManager::GetInstance().CreateFontTexture(
        "intro_message", "the quick brown fox jumps over the lazy dog.", mineFont, {255, 255, 255});


    LOG_INFO("Content %s ", textFile.c_str());

    LOG_INFO("Working flawlessly in %s ", SDL_GetPlatform());
    LOG_INFO("Version %d ", SDL_GetVersion());
    LOG_INFO("Video %s ", SDL_GetVideoDriver(0));
    LOG_INFO("GPU driver %s ", SDL_GetGPUDriver(0));
    LOG_INFO("RAM available %d ", SDL_GetSystemRAM());
    LOG_INFO("Screen Size %d %d ", width, height);


    SDL_FRect messageRect = {5, 5, message->w, message->h};

    SDL_FRect imageRect = {10, 10, 32, 32};

    *appstate = new GameContext {

        .messageTex = message, .messageDest = messageRect, .music = music, 
        .world = GWorld
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
            LOG_INFO("Resuming ");
            GFrameManager->Resume();
        } else {
            LOG_INFO("Pausing ");

            GFrameManager->Pause();
        }
    }

    return GEngine->engineState;
}

// WARNING: renderer order matter
SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = (GameContext*) appstate;


    GFrameManager->Update();


    const auto gameStatus = GFrameManager->IsPaused() ? "PAUSED" : "RUNNING";
    std::string title     = "[UNK_ENGINE] - Window - FPS: " + GFrameManager->GetFpsText() + gameStatus;

    char titleFmt[0x100];
    SDL_snprintf(titleFmt, sizeof(titleFmt), "[%s:%s] - Window Sample, [FPS]: %.2f, [GameState]: %s ", ENGINE_NAME,
                 ENGINE_VERSION_STR, GFrameManager->GetFps(), gameStatus);

    SDL_SetWindowTitle(GEngine->GetWindow(), titleFmt);


    SDL_SetRenderDrawColor(GEngine->GetRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

    SDL_RenderClear(GEngine->GetRenderer());


    app->world.each([&](flecs::entity e, TiledMap& map) {
        map.Render(GEngine->GetRenderer());

        e.children([&](flecs::entity child) {
            if (child.has<Entity>() && child.has<Sprite>() && child.has<Transform>()) {
                auto entity    = child.get_mut<Entity>();
                auto sprite    = child.get_mut<Sprite>();
                auto transform = child.get_mut<Transform>();


                sprite->SetPosition(transform->GetPosition());

                if (entity->type == EEntityType::PLAYER) {

                    HandlePlayerInput(transform, 20, GFrameManager->GetDeltaTime());
                }

                if (entity->type == EEntityType::ENEMY) {
                    const float FREQUENCY = 2.f;
                    float lerpFactor = (std::sin(GFrameManager->GetElapsedTime() * FREQUENCY * M_PI) + 1.0f) * 0.5f;

                    auto red   = 150 * (1 - lerpFactor) + 255 * lerpFactor;
                    auto green = 255 * (1 - lerpFactor) + 255 * lerpFactor;
                    auto blue  = 255 * (1 - lerpFactor) + 255 * lerpFactor;

                    sprite->SetColor(red, green, blue);

                    RenderHealthBar(GEngine->GetRenderer(), transform, entity);
                }

                sprite->Render(GEngine->GetRenderer());
            }
        });
    });

    // SDL_RenderTexture(GEngine->GetRenderer(), app->messageTex, NULL, &app->messageDest);


    SDL_SetRenderLogicalPresentation(GEngine->GetRenderer(), 320, 180, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    app->world.progress();

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

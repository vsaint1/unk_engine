#pragma once
#include <SDL3/SDL_time.h>
#include <SDL3/SDL_timer.h>
#include <memory>
#include <string>

class FrameManager {
    Uint64 lastTick    = SDL_GetPerformanceCounter();
    Uint64 currentTick = 0;
    float deltaTime    = 0.0;

    Uint64 frameCount = 0;
    float elapsedTime = 0.0;
    float fps         = 0.0;

    /* MAX_FPS = 1 -> UNLIMITED */
    Uint32 MAX_FPS    = 60;
    bool bIsPaused    = false;
    Uint64 pausedTick = 0;


public:
    FrameManager() {}

    float GetDeltaTime() {
        return this->deltaTime;
    }

    Uint64 GetFrameCount() {
        return this->frameCount;
    }

    float GetElapsedTime(){
        return this->elapsedTime;
    }

    void SetMaxFps(Uint32 fps) {
        this->MAX_FPS = fps;
    }

    float GetFps() {
        return this->fps;
    }

    std::string GetFpsText() {
        return std::to_string(this->fps);
    }

    void Pause() {
        if (!bIsPaused) {
            bIsPaused  = true;
            pausedTick = SDL_GetPerformanceCounter();
        }
    }

    void Resume() {
        if (bIsPaused) {
            bIsPaused         = false;
            Uint64 resumeTick = SDL_GetPerformanceCounter();
            lastTick += (resumeTick - pausedTick);
            pausedTick = 0;
        }
    }

    bool IsPaused() {
        return this->bIsPaused;
    }

    void Update();

    /* MAX_FPS = 1 -> UNLIMITED */
    void FixedFrameRate(Uint32 MAX_FPS = 60);
};

inline auto GFrameManager = std::make_unique<FrameManager>();

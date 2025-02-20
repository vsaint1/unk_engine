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
    Uint32 MAX_FPS = 60;

public:
FrameManager() {}

    float GetDeltaTime() {
        return this->deltaTime;
    }

    Uint64 GetFrameCount() {
        return this->frameCount;
    }

    void SetMaxFps(Uint32 fps) {
        this->MAX_FPS = fps;
    }

    float GetFps() {
        return this->fps;
    }

    std::string GetFpsText(){
        return std::to_string(this->fps);
    }

    void Update();

    void FixedFrameRate();

};

inline auto GFrameManager = std::make_unique<FrameManager>();
#include "../public/core/Timer.h"

void FrameManager::Update() {

    currentTick = SDL_GetPerformanceCounter();

    deltaTime = (float) (currentTick - lastTick) / SDL_GetPerformanceFrequency();
    lastTick  = currentTick;

    frameCount++;
    elapsedTime += deltaTime;

    if (elapsedTime >= 1.0) {
        fps         = frameCount / elapsedTime;
        frameCount  = 0;
        elapsedTime = 0.0;
    }
}

void FrameManager::FixedFrameRate() {

    if (MAX_FPS > 1) {
        float delay = 1000.0f / MAX_FPS;
        if (delay > 0) {
            SDL_Delay((Uint32) delay);
        }
    }
}

#pragma once

#include "core/Object.h"

class Camera : public GameObject {


    glm::vec2 position = {0, 0};

    glm::ivec2 viewport = {0, 0};

public:
    Camera(Uint32 width, Uint32 height, const float camera_speed, glm::vec2 viewport = {320, 180})
        : width(width), height(height), cameraSpeed(camera_speed), viewport(viewport) {}

    glm::vec2 GetViewport() const {
        return viewport;
    }

    glm::vec2 GetPosition() const {
        return position;
    }

    SDL_FRect GetViewBounds() {
        return {
            position.x,
            position.y, 
            static_cast<float>(width),
            static_cast<float>(height) 
        };
    }

    void SetPosition(const glm::vec2& position) {
        this->position = position;
    }

    void Awake() override {
        this->viewport = {320, 180};
        this->position = {0, 0};
    }

    void Start() override {}

    void Update(float delta_time) override {
        auto event = GameObject::event;

        auto state = SDL_GetKeyboardState(nullptr);
        glm::vec2 direction(0.0f, 0.0f);

        if (state[SDL_SCANCODE_W]) {
            LOG_INFO("test");
            direction.y -= cameraSpeed;
        }
        if (state[SDL_SCANCODE_S]) {
            direction.y += cameraSpeed;
        }
        if (state[SDL_SCANCODE_A]) {
            direction.x -= cameraSpeed;
        }
        if (state[SDL_SCANCODE_D]) {
            direction.x += cameraSpeed;
        }

        position += direction * cameraSpeed * delta_time;

        if (position.x < 0.0f) {
            position.x = 0.0f;
        }
        if (position.y < 0.0f) {
            position.y = 0.0f;
        }
        if (position.x > width - viewport.x) {
            position.x = width - viewport.x;
        }
        if (position.y > height - viewport.y) {
            position.y = height - viewport.y;
        }
    }

    // 320x180 |  640x360 | 1280x720 -> 13:9 |  16:9 |  4:3
    void Render(SDL_Renderer* renderer) override {

        GameObject::Render(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_FRect rect = {this->position.x, this->position.y, this->viewport.x, this->viewport.y};

        SDL_RenderRect(renderer, &rect);
    }
    

    void ProcessEvent(SDL_Event* event) override {
        GameObject::ProcessEvent(event);
    }

    void Destroy() override {}

protected:
    Uint32 width, height = 0;
    float cameraSpeed = 0.0f;
};

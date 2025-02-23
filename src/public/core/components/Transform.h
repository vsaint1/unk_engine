#pragma once
#include "core/Object.h"


class Transform {
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec2 scale    = {1, 1};

public:
    Transform() = default;

    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec2 scale) {

        this->position = position;
        this->rotation = rotation;
        this->scale    = scale;
    }


    glm::vec3 GetPosition()  {
        return position;
    }

    void SetPosition(const glm::vec3& position) {
        this->position = position;
    }

    glm::vec3 GetRotation()  {
        return rotation;
    }

    void SetRotation(const glm::vec3& rotation) {
        this->rotation = rotation;
    }

    void SetScale(const glm::vec2& scale) {
        this->scale = scale;
    }

    glm::vec2 GetScale()  {
        return scale;
    }
};

#pragma once
#include "core/Object.h"

class Sprite {
    SDL_Texture* texture = nullptr;
    glm::vec2 size       = {0, 0};
    glm::vec2 position   = {0, 0};
    float angle          = 0;
    glm::vec2 origin     = {0, 0};
    SDL_FlipMode flip    = SDL_FLIP_NONE;

public:

    
    Sprite() = default;

    Sprite(SDL_Texture* texture, glm::vec2 size, glm::vec2 position, glm::vec2 origin) {
        this->texture  = texture;
        this->size     = size;
        this->position = position;
        this->origin   = origin;
    }

    void Render(SDL_Renderer* renderer) {
        SDL_FRect rect = {position.x,position.y, size.x, size.y};
      
        SDL_FPoint point = {origin.x, origin.y};
        SDL_RenderTextureRotated(renderer, texture, nullptr, &rect, static_cast<double>(angle), &point, flip);
    }

    SDL_Texture* GetTexture() const {
        return texture;
    }

    void SetTexture(SDL_Texture*& texture) {
        this->texture = texture;
    }

    glm::vec2 GetPosition() const {
        return position;
    }

    void SetPosition(const glm::vec2& position) {
        this->position = position;
    }

    glm::vec2 GetSize() const {
        return size;
    }

    void SetSize(const glm::vec2& size) {
        this->size = size;
    }

    glm::vec2 GetOrigin() const {
        return origin;
    }

    void SetOrigin(const glm::vec2& origin) {
        this->origin = origin;
    }

    void SetColor(const Uint8& r, const Uint8& g, const Uint8& b) {
        SDL_SetTextureColorMod(this->texture, r,g,b);
    
    }

    void SetRotation(float angle) {
		this->angle = angle;
	}

    void Flip(const SDL_FlipMode flip_mode) {
       this->flip = flip_mode;
	}

};

#pragma once
#include "helpers/Logging.h"

#define FLECS_SOFT_ASSERT
#include "components/flecs.h"

/* BRIEF: Base class for all objects */
class GameObject {

public:
    /* BRIEF: Called when the object is created*/
    virtual void Awake() = 0;

    /* BRIEF: Called when the object is started*/
    virtual void Start() = 0;

    /* BRIEF: Called every tick in SDL_AppIterate */
    virtual void Update(float delta_time) = 0;

    /* BRIEF: Called every tick in SDL_AppIterate  `always call SUPER` */
    virtual void Render(SDL_Renderer* renderer){
        this->renderer = renderer;
    }

    /* BRIEF: Called every tick in SDL_AppEvent `always call SUPER` */
    virtual void ProcessEvent(SDL_Event* event){
        this->event = event;
    }

    /* BRIEF: Called when the object is destroyed*/
    virtual void Destroy() = 0;


protected:
    SDL_Event* event;
    SDL_Renderer* renderer;
};

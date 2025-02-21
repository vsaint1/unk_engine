#pragma once
#include "Imports.h"

#if __ANDROID__
const std::filesystem::path BASE_PATH = "";
#define ASSETS_PATH std::string("")
#elif __APPLE__
const std::filesystem::path BASE_PATH = SDL_GetBasePath();
#define ASSETS_PATH (BASE_PATH / "assets/").string()
#else
const std::filesystem::path BASE_PATH = SDL_GetBasePath();
#define ASSETS_PATH std::string("assets/")
#endif

#define ENGINE_NAME        "unk_engine"
#define ENGINE_VERSION_STR "0.0.1"

/*

BRIEF: This WILL `Enable` or `Disable` many features of the engine

if `BUILD_SHIPPING` is False the engine will be in development mode and enable the following features

- [ ] ImGui (TODO) 
- [ ] Logging (TODO)
- [ ] Loading asset files from `assets` folder and not from .pak files (TODO)

*/

#define BUILD_SHIPPING 0
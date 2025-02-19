#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_system.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include "helpers/Json.hpp"

#if defined(__EMSCRIPTEN__) || defined(__IOS__) || defined(__ANDROID__)
#include <SDL3/SDL_opengles2.h>
#define GLES 1
#else
#include <SDL3/SDL_opengl.h>
#define GLES 0
#endif

using Json = nlohmann::json; 
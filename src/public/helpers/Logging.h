#pragma once
#include "Macros.h"

#define LOG_ERROR(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_INFO(...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_DEBUG(...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_VERBOSE(...) SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_WARN(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_TRACE(...) SDL_LogTrace(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_CRITICAL(...) SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define LOG_QUIT_ON_FAIL(x) if(!x) { LOG_ERROR("%s", SDL_GetError()); return SDL_APP_FAILURE; }

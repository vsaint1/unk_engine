#pragma once
#include "Engine.h"


enum EScaleMode { LINEAR = 0, NEAREST };

enum EWriteMode { APPEND = 0, OVERWRITE };

enum class ETextRenderMode { SOLID, SHADED, BLENDED };


/* Load, Write and manage cross-platform `RESOURCES` */
class ResourceManager {
    std::unordered_map<std::string, SDL_Surface*> surfaces;
    std::unordered_map<std::string, SDL_Texture*> textures;

    std::unordered_map<std::string, TTF_Font*> fonts;
    std::unordered_map<std::string, Mix_Chunk*> sounds;
    std::unordered_map<std::string, Mix_Music*> music;

public:
    static ResourceManager& GetInstance();

    TTF_Font* GetFont(const std::string& path, int size);
    Mix_Music* GetMusic(const std::string& path);

    SDL_Surface* GetSurface(const std::string& path);

    SDL_Texture* GetTexture(const std::string& path);

    /* WARNING: FONT's must have `unique identifier's` to FIND and DRAW them*/
    SDL_Texture* CreateFontTexture(const std::string& uid, const std::string& text, TTF_Font* font, SDL_Color color,
                                   ETextRenderMode mode = ETextRenderMode::SOLID, SDL_Color bgColor = {0, 0, 0});

    std::string GetAssetsPath() {
        return ASSETS_PATH;
    }

    std::string LoadFromFile(const std::string& filename);

    void WriteToFile(const std::string& filename, const void* content, size_t size, EWriteMode mode = OVERWRITE);

    std::string GetExternalStorage() {
#if defined(__ANDROID__)
        return SDL_AndroidGetExternalStoragePath();
#elif defined(__APPLE__)
#if __IOS__
        NSArray* paths          = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsPath = [paths objectAtIndex:0];
        return std::string([documentsPath UTF8String]) + "/";
#else
        return std::string(getenv("HOME")) + "/Documents/";
#endif
#elif defined(_WIN32)
        char* userProfile = getenv("USERPROFILE");
        return std::string(userProfile) + "\\Documents\\";
#else
        return std::string(getenv("HOME")) + "/Documents/";
#endif
    }

private:
    ~ResourceManager();
    ResourceManager()                                  = default;
    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
};

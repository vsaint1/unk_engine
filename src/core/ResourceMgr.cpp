#include "../public/core/ResourceMgr.h"


ResourceManager& ResourceManager::GetInstance() {
    static ResourceManager* instance = new ResourceManager();
    return *instance;
}

TTF_Font* ResourceManager::GetFont(const std::string& path, int size) {
    if (path.empty()) {
        LOG_WARN("Path is empty!");
        return nullptr;
    }

    if (fonts.find(path) != fonts.end()) {
        return fonts[path];
    }

#if BUILD_SHIPPING == 0
    TTF_Font* font = TTF_OpenFont(this->GetAssetsPath().append(path).c_str(), size);

    if (!font) {
        LOG_ERROR("Failed to load font '%s': %s \n", path.c_str(), SDL_GetError());
        return nullptr;
    }
#else

    Pak pak;
    pak.LoadPakFile(ASSETS_PATH.append("assets.pak"));

    auto file = pak.ReadFile(path);

    void* mem_buffer = Memory::GetInstance()->Alloc(file.data(), file.size());

    SDL_IOStream* mem_rw = SDL_IOFromConstMem(mem_buffer, file.size());

    TTF_Font* font = TTF_OpenFontIO(mem_rw, 1, size);

#endif

    this->fonts[path] = font;

    return this->fonts[path];
}

Mix_Music* ResourceManager::GetMusic(const std::string& path) {
    if (path.empty()) {
        LOG_WARN("Path is empty!");
        return nullptr;
    }

    auto it = musics.find(path);
    if (it != musics.end()) {
        return it->second;
    }


#if BUILD_SHIPPING == 0
    Mix_Music* music = Mix_LoadMUS(this->GetAssetsPath().append(path).c_str());
    if (!music) {
        LOG_ERROR("Failed to load music '%s': %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }
#else
    Pak pak;
    pak.LoadPakFile(ASSETS_PATH.append("assets.pak"));
    auto file = pak.ReadFileToMemory(path);

    void* mem_buffer = Memory::GetInstance()->Alloc(file.data(), file.size());

    SDL_IOStream* mem_rw = SDL_IOFromConstMem(mem_buffer, file.size());

    Mix_Music* music = Mix_LoadMUS_IO(mem_rw, 0);

#endif

    musics[path] = music;
    return music;
}

SDL_Surface* ResourceManager::GetSurface(const std::string& path) {


    if (path.empty()) {
        LOG_WARN("Path is empty!");
        return nullptr;
    }

    if (surfaces.find(path) != surfaces.end()) {
        return surfaces[path];
    }


#if BUILD_SHIPPING == 0
    SDL_Surface* surface = IMG_Load(this->GetAssetsPath().append(path).c_str());
    if (!surface) {
        LOG_ERROR("Failed to load surface '%s': %s \n", path.c_str(), SDL_GetError());
        return nullptr;
    }
#else
    Pak pak;
    pak.LoadPakFile(ASSETS_PATH.append("assets.pak"));

    auto file = pak.ReadFile(path);

    auto imageIO         = SDL_IOFromMem(file.data(), file.size());
    SDL_Surface* surface = IMG_Load_IO(imageIO, 1); // AUTO CLOSABLE

#endif

    this->surfaces[path] = surface;
    return surface;
}

SDL_Texture* ResourceManager::GetTexture(const std::string& path) {

    if (this->textures.count(path) == 0) {
        SDL_Surface* surface = this->GetSurface(path);
        if (!surface) {
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(GEngine->GetRenderer(), surface);
        if (!texture) {
            LOG_ERROR("Failed to create texture from '%s': %s", path.c_str(), SDL_GetError());
            return nullptr;
        }

        SDL_SetTextureScaleMode(texture, engine::bEnableScaleModeNearest  ? SDL_SCALEMODE_NEAREST : SDL_SCALEMODE_LINEAR);

        this->textures[path] = texture;
    }

    return this->textures[path];
}

SDL_Texture* ResourceManager::CreateFontTexture(const std::string& uid, const std::string& text, TTF_Font* font,
                                                SDL_Color color, ETextRenderMode mode, SDL_Color bgColor) {

    SDL_Surface* surface = nullptr;

    if (!font) {
        LOG_ERROR("Invalid font, will continue using the default \n");
    } else {
        // If this fail we just get the `DEFAULT` font
        font = fonts.begin()->second;
    }

    switch (mode) {
    case ETextRenderMode::SOLID:
        surface = TTF_RenderText_Solid(font, text.c_str(), text.length(), color);
        break;
    case ETextRenderMode::SHADED:
        surface = TTF_RenderText_Shaded(font, text.c_str(), text.length(), color, bgColor);
        break;
    case ETextRenderMode::BLENDED:
        surface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
        break;
    default:
        LOG_CRITICAL("Unknown render_mode Selected, check the ETextRenderMode enum");
        return nullptr;
    }

    if (!surface) {
        LOG_ERROR("Failed to create text surface '%s': %s, unique_identifier %s \n", text.c_str(), SDL_GetError(),
                  uid.c_str());
        return nullptr;
    }

    if (this->surfaces.find(uid) != this->surfaces.end()) {
        LOG_WARN("Surface with unique identifier '%s' already exists, will override \n", uid.c_str());
    }

    this->surfaces[uid] = surface;

    if (textures.find(uid) != textures.end()) {
        SDL_DestroyTexture(textures[uid]);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(GEngine->GetRenderer(), surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        LOG_ERROR("Failed to create texture from text: %s\n", SDL_GetError());
        return nullptr;
    }

    this->textures[uid] = texture;

    return texture;
}


/* BRIEF: this will load a file and return the content */
std::string ResourceManager::LoadFromFile(const std::string& filename) {

#if BUILD_SHIPPING == 0
    size_t filesize = 0;
    auto filePath   = this->GetAssetsPath().append(filename);

    if (filePath.empty()) {
        LOG_WARN("Not found in assets, trying external storage");
        filePath = this->GetExternalStorage().append(filename);

        if (filePath.empty()) {
            LOG_ERROR("Failed to load file '%s': %s \n", filename.c_str(), SDL_GetError());
            return "";
        }
    }


    // TODO: change this to SDL_LoadFile_IO
    void* data = SDL_LoadFile(filePath.c_str(), &filesize);

    if (!data) {
        LOG_ERROR("Failed to load file '%s': %s \n", filename.c_str(), SDL_GetError());
        return "";
    }

    std::string content(static_cast<char*>(data), filesize);
    SDL_free(data);

    return content;

#else
    Pak pak;
    pak.LoadPakFile(ASSETS_PATH.append("assets.pak"));

    return pak.ReadFile(filename);

#endif
}

void ResourceManager::WriteToFile(const std::string& filename, const void* content, size_t size, EWriteMode mode,
                                  const std::string& folder) {

    auto filePath = this->GetExternalStorage(folder.c_str()).append(filename);

    auto file = SDL_IOFromFile(filePath.c_str(), mode == OVERWRITE ? "wb" : "ab");

    if (!file) {
        LOG_ERROR("Failed to open file '%s': %s ", filename.c_str(), SDL_GetError());
        return;
    }

    size_t written = SDL_WriteIO(file, content, size);

    if (written != size) {
        LOG_ERROR("Failed to write entire content to file '%s': %s\n", filename.c_str(), SDL_GetError());
    }

    LOG_INFO("Saved file at %s, with size %lu", filePath.c_str(), size);
    SDL_CloseIO(file);
}

ResourceManager::~ResourceManager() {


    for (auto& [key, font] : fonts) {
        TTF_CloseFont(font);
    }

    fonts.clear();

    for (auto& [key, surface] : surfaces) {
        SDL_DestroySurface(surface);
    }

    surfaces.clear();

    for (auto& [key, texture] : textures) {
        SDL_DestroyTexture(texture);
    }

    textures.clear();

    for (auto& [key, music] : musics) {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
    }

    Memory::GetInstance()->Cleanup();

    musics.clear();
}

#pragma once
#include "ResourceMgr.h"
#include "helpers/Utilities.h"

struct CollisionObject {
    float x, y, width, height;
};

class TileLayer {
    Uint32 id;
    std::vector<Uint32> tiles;
    Uint32 width;
    Uint32 height;
    std::string name;

public:
    TileLayer() = default;

    void DecodeTile(const std::string& encoded_data, int map_width, int map_height);

    std::vector<Uint32> GetTiles() const {
        return tiles;
    }

    std::string GetName() const {
        return name;
    }

    void SetID(Uint32 id) {
        this->id = id;
    }

    void SetName(const std::string& name) {
        this->name = name;
    }

    void SetWidth(int width) {
        this->width = width;
    }

    void SetHeight(int height) {
        this->height = height;
    }


    Uint32 GetID() const {
        return id;
    }

    Uint32 GetWidth() const {
        return width;
    }

    Uint32 GetHeight() const {
        return height;
    }

private:
};

struct Tileset {
    Uint32 firstgid;
    SDL_Texture* texture;
    std::string name;
    Uint32 columns;
    Uint32 tileCount;
};

/* WARNING: ONCE EXPORTED BY TILED CHANGE THE IMAGE `SOURCE` ON THE XML FILE */
class TiledMap {
public:
    std::vector<TileLayer> layers;
    std::vector<Tileset> tilesets;
    std::vector<CollisionObject> collisions;
    Uint32 tileSize = 16;
    std::string name;
    std::string source;

    SDL_Texture* texture;

    TiledMap() = default;

    TiledMap(const std::string& mapPath) {
        this->mapPath = mapPath;
    }

    void Awake();

    void Render(SDL_Renderer* renderer);

    void SetVersion(const std::string& version) {
        tiledVersion = version;
    }

    void SetTileSize(Uint32 tileSize) {
        this->tileSize = tileSize;
    }

    void SetSource(const std::string& source) {
        this->source = source;
    }

    Uint16 GetRenderOrder() const {
        return renderOrder;
    }
    Uint16 GetCompressionLevel() const {
        return compressionLevel;
    }
    std::string GetTiledVersion() const {
        return tiledVersion;
    }


protected:
    void AddTileset(Uint32 firstgid, SDL_Texture* texture, const std::string& name, Uint32 columns, Uint32 tileCount) {
        tilesets.push_back({firstgid, texture, name, columns, tileCount});
    }

    void ParseObjectGroup(XMLElement* objectGroup);

    std::string mapPath      = "";
    Uint16 renderOrder       = 0;
    Uint16 compressionLevel  = 0;
    std::string tiledVersion = "unknown_version";
};

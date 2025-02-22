#include "../public/core/components/TileMgr.h"

/* zlib decompression, currently not using. need to fix emscripten port */
#if UNK_USE_ZLIB
std::vector<Uint32> Decode_Base64_zlib(const std::string& encoded, int size) {
    std::string decoded_base64 = decode_base64_and_inflate(encoded);

    uLongf dest_len = size * 4;
    std::vector<unsigned char> decompressed(dest_len);

    int result =
        uncompress(decompressed.data(), &dest_len, (const Bytef*) decoded_base64.data(), decoded_base64.size());
    if (result != Z_OK) {
        LOG_ERROR("Failed to decompress data: %d", result);
        return {};
    }

    std::vector<Uint32> tiles(dest_len / 4);
    for (size_t i = 0; i < tiles.size(); ++i) {
        tiles[i] = decompressed[i * 4] | (decompressed[i * 4 + 1] << 8) | (decompressed[i * 4 + 2] << 16)
                 | (decompressed[i * 4 + 3] << 24);
    }
    return tiles;
}
#endif

void TileLayer::DecodeTile(const std::string& encoded_data, int map_width, int map_height) {
    std::string decoded = Decode_Base64(encoded_data);

    size_t tile_count = map_width * map_height;
    std::vector<Uint32> tiles(tile_count);

    for (size_t i = 0; i < tile_count; i++) {
        tiles[i] = static_cast<unsigned char>(decoded[i * 4]) | (static_cast<unsigned char>(decoded[i * 4 + 1]) << 8)
                 | (static_cast<unsigned char>(decoded[i * 4 + 2]) << 16)
                 | (static_cast<unsigned char>(decoded[i * 4 + 3]) << 24);
    }

    this->tiles = tiles;
}

void TiledMap::Awake() {
    XMLDocument document;

    auto content = ResourceManager::GetInstance().LoadFromFile(mapPath);

    document.Parse(content.c_str(), content.length());

#if _DEBUG
    XMLPrinter printer;

    document.Print(&printer);
    LOG_INFO("Map %s, XML_Data: %s \n", this->mapPath.c_str(), printer.CStr());

#endif

    XMLElement* mapElement = document.FirstChildElement("map");

    if (!mapElement) {
        LOG_WARN("Unknown map format expected -> [TILED]");
        return;
    }

    // Parse tilesets
    for (XMLElement* tilesetElement = mapElement->FirstChildElement("tileset"); tilesetElement;
         tilesetElement             = tilesetElement->NextSiblingElement("tileset")) {

        Uint32 firstgid         = tilesetElement->UnsignedAttribute("firstgid");
        std::string tilesetName = tilesetElement->Attribute("name");
        Uint32 columns          = tilesetElement->UnsignedAttribute("columns");
        Uint32 tileCount        = tilesetElement->UnsignedAttribute("tilecount");

        XMLElement* imageElement = tilesetElement->FirstChildElement("image");

        if (!imageElement) {
            LOG_WARN("Tileset %s has no image element.", tilesetName.c_str());
            continue;
        }

        std::string imagePath = imageElement->Attribute("source");
        auto tilesetTexture   = ResourceManager::GetInstance().GetTexture(imagePath);

        if (!tilesetTexture) {
            LOG_ERROR("Failed to load tileset texture: %s", imagePath.c_str());
            continue;
        } else {
            LOG_INFO("Loaded tileset texture: %s", imagePath.c_str());
        }

        this->AddTileset(firstgid, tilesetTexture, tilesetName, columns, tileCount);
        LOG_INFO("Loaded tileset %s (firstgid: %d) \n", tilesetName.c_str(), firstgid);
    }

    this->SetVersion(mapElement->Attribute("tiledversion"));

    std::vector<TileLayer> layers;
    layers.clear();

    for (XMLElement* layerElement = mapElement->FirstChildElement("layer"); layerElement;
         layerElement             = layerElement->NextSiblingElement("layer")) {

        XMLElement* data = layerElement->FirstChildElement("data");

        if (!data || !data->GetText()) {
            continue;
        }

        auto encoding = data->Attribute("encoding");

        if (!SDL_strstr(encoding, "base64")) {
            LOG_WARN("Unsupported encoding %s. Supported encodings are [base64].\n", encoding);
            break;
        }

        LOG_INFO("Encoding: %s \n", encoding);

        std::string name = layerElement->Attribute("name");

        LOG_INFO("Loading layer: %s \n", name.c_str());

        int id = layerElement->IntAttribute("id");

        TileLayer layer{};
        layer.SetID(id);
        layer.SetName(name);
        layer.SetWidth(mapElement->IntAttribute("width"));
        layer.SetHeight(mapElement->IntAttribute("height"));

        std::string encoded = data->GetText();
        layer.DecodeTile(encoded, layer.GetWidth(), layer.GetHeight());

        layers.emplace_back(std::move(layer));
        LOG_INFO("Loaded layer: %s \n", name.c_str());
    }

    this->layers = layers;

    for (XMLElement* objectGroup = mapElement->FirstChildElement("objectgroup"); objectGroup;
         objectGroup             = objectGroup->NextSiblingElement("objectgroup")) {

        LOG_INFO("Loading object group: %s \n", objectGroup->Attribute("name"));
        ParseObjectGroup(objectGroup);
    }
}

void TiledMap::ParseObjectGroup(XMLElement* objectGroup) {
    if (!objectGroup) {
        return;
    }

    for (XMLElement* object = objectGroup->FirstChildElement("object"); object;
         object             = object->NextSiblingElement("object")) {
        float x      = object->FloatAttribute("x");
        float y      = object->FloatAttribute("y");
        float width  = object->FloatAttribute("width");
        float height = object->FloatAttribute("height");
        LOG_INFO("Collision object: x: %f, y: %f, width: %f, height: %f", x, y, width, height);
        this->collisions.push_back({x, y, width, height});
    }

    LOG_INFO("Loaded %d collision objects.", static_cast<int>(collisions.size()));
}

/* BRIEF: We render each layer (currently this is only for uniform sized tiles ex: (16x16,32x32))*/
void TiledMap::Render(SDL_Renderer* renderer) {
    if (layers.empty() || tilesets.empty()) {
        return;
    }


    for (const auto& layer : layers) {
        const auto& tiles = layer.GetTiles();
        if (tiles.empty()) {
            continue;
        }

        Uint32 layerWidth  = layer.GetWidth();
        Uint32 layerHeight = layer.GetHeight();

        for (Uint32 y = 0; y < layerHeight; ++y) {
            for (Uint32 x = 0; x < layerWidth; ++x) {
                Uint32 tileID = tiles[y * layerWidth + x];

                if (tileID == 0) {
                    continue;
                }

                for (const auto& tileset : tilesets) {
                    if (tileID >= tileset.firstgid && tileID < tileset.firstgid + tileset.tileCount) {
                        Uint32 localTileID = tileID - tileset.firstgid;

                        Uint32 tileX = (localTileID % tileset.columns) * tileSize;
                        Uint32 tileY = (localTileID / tileset.columns) * tileSize;

                        SDL_FRect srcRect = {static_cast<float>(tileX), static_cast<float>(tileY),
                                             static_cast<float>(tileSize), static_cast<float>(tileSize)};

                        SDL_FRect destRect = {static_cast<float>(x * tileSize), static_cast<float>(y * tileSize),
                                              static_cast<float>(tileSize), static_cast<float>(tileSize)};


                        SDL_RenderTexture(renderer, tileset.texture, &srcRect, &destRect);

                        break;
                    }
                }
            }
        }
    }

    if (engine::bEnableRenderDebug) {

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        for (const auto& obj : collisions) {
            SDL_FRect rect = {obj.x, obj.y, obj.width, obj.height};

            SDL_RenderRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
}

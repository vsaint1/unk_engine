#include "../public/helpers/PakLoader.h"


std::string Pak::NormalizePath(const std::string& path) const {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

bool Pak::LoadPakFile(const std::string& pakFilePath) {
    SDL_IOStream* rw = SDL_IOFromFile(pakFilePath.c_str(), "rb");
    if (!rw) {
        LOG_ERROR("Failed to open PAK file: %s", SDL_GetError());
        return false;
    }

    Sint64 filesize = SDL_GetIOSize(rw);
    if (filesize <= 0) {
        LOG_ERROR("Invalid PAK file size.");
        SDL_CloseIO(rw);
        return false;
    }

    pakData.resize(filesize);
    if (SDL_ReadIO(rw, pakData.data(), filesize) != filesize) {
        LOG_ERROR("Failed to read PAK file.");
        SDL_CloseIO(rw);
        return false;
    }

    SDL_CloseIO(rw);

    if (pakData.size() < 8) {
        LOG_ERROR("Invalid PAK file format.");
        return false;
    }

    uint32_t indexSize;
    std::memcpy(&indexSize, pakData.data(), sizeof(uint32_t));

    size_t indexStart = pakData.size() - indexSize;
    size_t offset     = indexStart;

    while (offset < pakData.size()) {
        uint32_t pathLen;
        std::memcpy(&pathLen, &pakData[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::string path(&pakData[offset], pathLen);
        offset += pathLen;

        path = NormalizePath(path);  

        CFileEntry entry;
        std::memcpy(&entry.start, &pakData[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);
        std::memcpy(&entry.size, &pakData[offset], sizeof(uint32_t));
        offset += sizeof(uint32_t);

        index[path] = entry;
    }

    return true;
}

std::vector<std::string> Pak::ListFiles() const {
    std::vector<std::string> files;
    for (const auto& [path, _] : index) {
        LOG_INFO("file: %s\n", path.c_str());
        files.push_back(path);
    }
    return files;
}

std::string Pak::ReadFile(const std::string& filename) const {
    std::string normalizedFilename = NormalizePath(filename);

    auto it = index.find(normalizedFilename);
    if (it == index.end()) {
        LOG_ERROR("File not found: %s\n", normalizedFilename.c_str());
        return "";
    }

    const auto& entry = it->second;
    if (entry.start + entry.size > pakData.size()) {
        LOG_ERROR("Invalid file size for: %s\n", normalizedFilename.c_str());
        return "";
    }

    return std::string(&pakData[entry.start], entry.size);
}

std::vector<char> Pak::ReadFileToMemory(const std::string& filename) const {
    auto it = index.find(NormalizePath(filename));
    if (it == index.end()) {
        LOG_ERROR("File not found in PAK: %s\n", filename.c_str());
        return {};
    }

    const CFileEntry& entry = it->second;
    if (entry.start + entry.size > pakData.size()) {
        LOG_ERROR("Invalid file size or corrupted PAK entry for: %s\n", filename.c_str());
        return {};
    }

    // Allocate memory and read the file data
    std::vector<char> buffer(entry.size);
    std::memcpy(buffer.data(), pakData.data() + entry.start, entry.size);

    return buffer;
}
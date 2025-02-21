#pragma once
#include "Logging.h"

class Pak {
    public:
        struct CMetaData {
            std::string version;
            std::string name;
        };
    
        struct CFileEntry {
            std::string path;
            Uint32 start;
            Uint32 size;
        };
    
        bool LoadPakFile(const std::string& pakFilePath);

        std::vector<std::string> ListFiles() const;

        std::string ReadFile(const std::string& filename) const;

        std::vector<char> GetPakData() const { return pakData; }

        std::vector<char> ReadFileToMemory(const std::string& filename) const;

    private:    
        std::unordered_map<std::string, CFileEntry> index;
        std::vector<char> pakData;
    
        std::string NormalizePath(const std::string& path) const;
    };
    
    
    
#pragma once

#include <optional>
#include <filesystem>
#include <vector>
#include "Map.hpp"

struct Lump{
    uint32_t size;
    char name[9];
    std::vector<std::byte> data;
};

struct WAD{
    char id[5];
    uint32_t numLumps;
    std::vector<Lump> lumps;

    static std::optional<WAD> loadFromFile(const std::filesystem::path& filePath);
    static int findLump(std::string_view lumpName, const WAD& wadFile);
    static std::optional<Map> readMap(std::string_view mapName, const WAD& wadFile);
};


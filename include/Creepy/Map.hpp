#pragma once

#include <vector>
#include <glm/glm.hpp>

enum class LineDefFormat : uint16_t{
    PLAYER = 0x0001,
    MONSTER = 0x0002,
    TWO_SIDE = 0x0004,
    SECRET = 0x0020
};

struct LineDef{
    uint16_t startIndex{}, endIndex{};
    uint16_t flags{};
    uint16_t frontSideDef{}, backSideDef{};
};

struct SideDef{
    uint16_t sectorIndex{};
};

struct Sector{
    int16_t floor{}, ceiling{};
    int16_t lightLevel{};
};

struct Map{
    std::vector<glm::vec2> vertices;
    glm::vec2 min, max;
    std::vector<LineDef> lineDefs;
    std::vector<SideDef> sideDefs;
    std::vector<Sector> sectors;
};
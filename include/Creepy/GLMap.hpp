#pragma once

#include <vector>
#include <glm/glm.hpp>

struct GLSubSector{
    uint16_t numSegments;
    uint16_t firstSegment;
};

struct GLSegment{
    uint16_t startVertex{}, endVertex{};
    uint16_t lineDef{}, side{};
};

struct GLMap
{
    std::vector<glm::vec2> vertices;
    std::vector<GLSegment> segments;
    std::vector<GLSubSector> subSectors;
    glm::vec2 min, max;
};

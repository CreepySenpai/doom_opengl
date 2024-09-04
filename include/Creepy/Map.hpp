#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Map{
    std::vector<glm::vec2> vertices;
    glm::vec2 min, max;
};
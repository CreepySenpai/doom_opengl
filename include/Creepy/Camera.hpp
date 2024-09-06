#pragma once

#include <glm/glm.hpp>

struct Camera{
    glm::vec3 Position;
    float Pitch{}, Yaw{};

    glm::vec3 Forward, Right, Up;

    static void UpdateDirection(Camera& camera);
};
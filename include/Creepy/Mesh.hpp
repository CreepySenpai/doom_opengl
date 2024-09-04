#pragma once

#include <span>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex{
    glm::vec3 Position;
};

struct Mesh{
    GLuint VAO{}, VBO{}, EBO{};
    uint32_t NumIndices{};

    static Mesh createMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices);
};
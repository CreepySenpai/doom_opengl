#pragma once

#include <glm/glm.hpp>

struct Renderer{
    static void initRenderer(int width, int height);

    static void clearRenderer(glm::vec3 eye);

    static void drawPoint(glm::vec2 point, float size, const glm::vec4& color);
    static void drawLine(glm::vec2 point0, glm::vec2 point1, float lineWidth, const glm::vec4& color);
    static void drawQuad(glm::vec2 center, glm::vec2 size, float angle, const glm::vec4& color);
};
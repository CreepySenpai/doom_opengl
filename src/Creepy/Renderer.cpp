#include <print>
#include <cmath>
#include <Creepy/Renderer.hpp>
#include <Creepy/Utils.hpp>
#include <Creepy/Mesh.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>


float s_width{}, s_height{};
GLuint s_program{};
GLuint s_modelMatrixLocation{};
GLuint s_viewMatrixLocation{};
GLuint s_projectionMatrixLocation{};
GLuint s_colorLocation{};
Mesh s_quadMesh{};

static void initShaders();
static void initQuad();

void Renderer::initRenderer(int width, int height) {
    s_width = static_cast<float>(width);
    s_height = static_cast<float>(height);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    initShaders();
    initQuad();
}

void Renderer::clearRenderer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setViewMatrix(const glm::mat4& viewMatrix) {
    glUniformMatrix4fv(s_viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void Renderer::setProjectionMatrix(const glm::mat4& projectionMatrix) {
    glUniformMatrix4fv(s_projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

glm::ivec2 Renderer::getSize() {
    return {s_width, s_height};
}

void initShaders() {
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, readShaderFile("./res/shader/vertex.vert"));
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, readShaderFile("./res/shader/fragment.frag"));

    s_program = linkProgram(std::array{vertexShader, fragmentShader});

    glUseProgram(s_program);

    s_modelMatrixLocation = glGetUniformLocation(s_program, "modelMatrix");
    s_viewMatrixLocation = glGetUniformLocation(s_program, "viewMatrix");
    s_projectionMatrixLocation = glGetUniformLocation(s_program, "projectionMatrix");
    s_colorLocation = glGetUniformLocation(s_program, "myColor");
}

void initQuad() {
    constexpr Vertex vertices[]{
        glm::vec3{1.0f, 1.0f, 0.0f},
        glm::vec3{0.0f, 1.0f, 0.0f},
        glm::vec3{0.0f, 0.0f, 0.0f},
        glm::vec3{1.0f, 0.0f, 0.0f}
    };

    constexpr uint32_t indices[]{
        0, 1, 3, 
        1, 2, 3
    };

    s_quadMesh = Mesh::createMesh(vertices, indices);
}

void Renderer::drawPoint(glm::vec2 point, float size, const glm::vec4& color) {
    const glm::mat4 translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{point.x, point.y, 0.0f});
    const glm::mat4 scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{size, size, 1.0f});

    drawMesh(s_quadMesh, translationMatrix * scaleMatrix, color);
}

void Renderer::drawLine(glm::vec2 point0, glm::vec2 point1, float lineWidth, const glm::vec4& color) {
    const float x = point1.x - point0.x;
    const float y = point1.y - point0.y;
    const float r = std::sqrt(x * x + y * y);
    const float angle = std::atan2(y, x);   // Angle x ^ y, already in rad

    // Translate To Mid Point
    const glm::mat4 translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{(point0.x + point1.x) / 2.0f, (point0.y + point1.y) / 2.0f, 0.0f});
    const glm::mat4 scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{r, lineWidth, 1.0f});
    const glm::mat4 rotationMatrix = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3{0.0f, 0.0f, 1.0f});

    drawMesh(s_quadMesh, translationMatrix * rotationMatrix * scaleMatrix, color);
}

void Renderer::drawQuad(glm::vec2 center, glm::vec2 size, float angle, const glm::vec4& color) {
    const glm::mat4 translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{center.x, center.y, 0.0f});
    const glm::mat4 scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{size.x, size.y, 1.0f});
    const glm::mat4 rotationMatrix = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3{0.0f, 0.0f, 1.0f});

    drawMesh(s_quadMesh, translationMatrix * rotationMatrix * scaleMatrix, color);
}

void Renderer::drawMesh(const Mesh& mesh, const glm::mat4& transform, const glm::vec4& color) {
    glUniform4fv(s_colorLocation, 1, glm::value_ptr(color));
    glUniformMatrix4fv(s_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glDrawElements(GL_TRIANGLES, mesh.NumIndices, GL_UNSIGNED_INT, nullptr);
}
#include <print>
#include <cmath>
#include <Creepy/Renderer.hpp>
#include <Creepy/Utils.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>

float s_width{}, s_height;
GLuint s_program{};
GLuint s_modelMatrixLocation{};
GLuint s_colorLocation{};

void initShaders();
void initQuad();
void initProjection();

void Renderer::initRenderer(int width, int height) {
    s_width = static_cast<float>(width);
    s_height = static_cast<float>(height);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    initShaders();
    initQuad();
    initProjection();
}

void Renderer::clearRenderer(glm::vec3 eye) {
    glClear(GL_COLOR_BUFFER_BIT);
    glm::mat4 view = glm::lookAtLH(eye, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    const GLuint viewLoc = glGetUniformLocation(s_program, "viewMatrix");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void initShaders() {
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, readShaderFile("./res/shader/vertex.vert"));
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, readShaderFile("./res/shader/fragment.frag"));

    s_program = linkProgram(std::array{vertexShader, fragmentShader});

    glUseProgram(s_program);

    s_modelMatrixLocation = glGetUniformLocation(s_program, "modelMatrix");
    s_colorLocation = glGetUniformLocation(s_program, "myColor");
}

void initQuad() {
    constexpr float vertices[]{
        0.5f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5f, 0.5f,
    };

    constexpr uint32_t indices[]{
        0, 1, 3, 
        1, 2, 3
    };

    GLuint VAO{}, VBO{}, EBO{};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);      // unbind
}

void initProjection() {
    auto pro = glm::orthoLH(0.0f, s_width, 0.0f, s_height, -100.0f, 100.0f);
    const GLuint projectionUniformLoc = glGetUniformLocation(s_program, "projectionMatrix");
    glUniformMatrix4fv(projectionUniformLoc, 1, GL_FALSE, glm::value_ptr(pro));
}

void Renderer::drawPoint(glm::vec2 point, float size, const glm::vec4& color) {
    const glm::mat4 tran = glm::translate(glm::identity<glm::mat4>(), glm::vec3{point.x, point.y, 0.0f});
    const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3{size, size, 1.0f});
    
    auto mo = tran * scale;

    glUniform4fv(s_colorLocation, 1, glm::value_ptr(color));
    glUniformMatrix4fv(s_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(mo));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::drawLine(glm::vec2 point0, glm::vec2 point1, float lineWidth, const glm::vec4& color) {
    const float x = point1.x - point0.x;
    const float y = point1.y - point0.y;
    const float r = std::sqrt(x * x + y * y);
    const float angle = std::atan2(y, x);   // Angle x ^ y, already in rad

    // Translate To Mid Point
    const glm::mat4 tran = glm::translate(glm::identity<glm::mat4>(), glm::vec3{(point0.x + point1.x) / 2.0f, (point0.y + point1.y) / 2.0f, 0.0f});
    const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3{r, lineWidth, 1.0f});
    const glm::mat4 rot = glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3{0.0f, 0.0f, 1.0f});

    auto mo = tran * rot * scale;

    glUniform4fv(s_colorLocation, 1, glm::value_ptr(color));
    glUniformMatrix4fv(s_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(mo));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void Renderer::drawQuad(glm::vec2 center, glm::vec2 size, float angle, const glm::vec4& color) {
    const glm::mat4 tran = glm::translate(glm::identity<glm::mat4>(), glm::vec3{center.x, center.y, 0.0f});
    const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3{size.x, size.y, 1.0f});
    const glm::mat4 rot = glm::rotate(glm::identity<glm::mat4>(), glm::radians(angle), glm::vec3{0.0f, 0.0f, 1.0f});

    auto mo = tran * rot * scale;

    glUniform4fv(s_colorLocation, 1, glm::value_ptr(color));
    glUniformMatrix4fv(s_modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(mo));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
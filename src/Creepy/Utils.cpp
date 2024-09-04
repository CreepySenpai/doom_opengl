#include <print>
#include <fstream>
#include <Creepy/Utils.hpp>

std::string readShaderFile(const std::filesystem::path& filePath){
    std::ifstream fileIn{filePath};
    return {std::istreambuf_iterator<char>{fileIn}, std::istreambuf_iterator<char>{}};
}

GLuint compileShader(GLenum type, std::string_view src) {
    GLuint shader = glCreateShader(type);

    auto data = src.data();
    glShaderSource(shader, 1, &data, nullptr);
    glCompileShader(shader);
    GLint success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success){
        char buffer[512];
        glGetShaderInfoLog(shader, sizeof(buffer), nullptr, buffer);
        std::println("Failed: {}", buffer);
    }

    return shader;
}

GLuint linkProgram(std::span<const GLuint> shaders){
    GLuint64 program = glCreateProgram();

    for(auto shader : shaders){
        glAttachShader(program, shader);
    }

    glLinkProgram(program);

    GLint success{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success){
        char buffer[512];
        glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
        std::println("Failed: {}", buffer);
    }

    return program;
}
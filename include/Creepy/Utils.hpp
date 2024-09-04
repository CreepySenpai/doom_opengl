#pragma once

#include <filesystem>
#include <string_view>
#include <span>
#include <glad/glad.h>


std::string readShaderFile(const std::filesystem::path& filePath);

GLuint compileShader(GLenum type, std::string_view src);

GLuint linkProgram(std::span<const GLuint> shaders);
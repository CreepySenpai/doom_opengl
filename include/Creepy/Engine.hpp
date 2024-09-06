#pragma once

#include <string_view>

struct Engine{

    static void Init(const struct WAD& wadFile, std::string_view mapName);
    static void Update(float deltaTime);
    static void Render();
};
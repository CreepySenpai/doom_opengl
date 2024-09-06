#include <print>
#include <utility>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Creepy/Renderer.hpp>
#include <Creepy/WAD.hpp>
#include <Creepy/Engine.hpp>
#include <Creepy/Input.hpp>

int main(){
    int width{600}, height{600};
    if(glfwInit() != GLFW_TRUE){
        std::println("Failed Init");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    auto window = glfwCreateWindow(width, height, "Doom", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::println("Failed Init Glad");
    }

    Renderer::initRenderer(width, height);

    float lastTime{0.0f};

    auto wadFile = WAD::loadFromFile("./res/levels/doom1.wad");

    Engine::Init(wadFile.value(), "E1M1");

    Input::Init(window);

    while(!glfwWindowShouldClose(window)){
        float nowTime = glfwGetTime();
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;

        glfwPollEvents();

        Engine::Update(deltaTime);

        Renderer::clearRenderer();
        
        Engine::Render();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
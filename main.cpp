#include <print>
#include <utility>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Creepy/Renderer.hpp>
#include <Creepy/WAD.hpp>

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

    static glm::vec3 eye{0.0f, 0.0f, -1.0f};

    auto wadFile = WAD::loadFromFile("./res/levels/doom1.wad");

    // for(auto& l : wadFile.value().lumps){
    //     std::println("{} - {}", l.name, l.size);
    // }

    auto mapFile = WAD::readMap("E1M1", wadFile.value());

    glm::vec2 outMin{20.0f, 20.0f};
    glm::vec2 outMax{width - 10.0f, height - 10.0f};

    std::vector<glm::vec2> reMapVertex(mapFile.value().vertices.size());

    for(size_t i{}; i < reMapVertex.size(); ++i){
        reMapVertex.at(i) = glm::vec2{
            (std::max(mapFile.value().min.x, std::min(mapFile.value().vertices.at(i).x, mapFile.value().max.x)) - mapFile.value().min.x) * (outMax.x - outMin.x) / (mapFile.value().max.x - mapFile.value().min.x) + outMin.x,
            height - (std::max(mapFile.value().min.y, std::min(mapFile.value().vertices.at(i).y, mapFile.value().max.y)) - mapFile.value().min.y) * (outMax.y - outMin.y) / (mapFile.value().max.y - mapFile.value().min.y) + outMin.y
        };
    }


    while(!glfwWindowShouldClose(window)){
        float nowTime = glfwGetTime();
        float deltaTime = nowTime - lastTime;
        lastTime = nowTime;

        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
            eye.z += 0.2f;
            std::println("{} - {} - {}", eye.x, eye.y, eye.z);
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
            eye.z -= 0.2f;
            std::println("{} - {} - {}", eye.x, eye.y, eye.z);
        }
        
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
            eye.x += 0.2f;
            std::println("{} - {} - {}", eye.x, eye.y, eye.z);
        }

        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
            eye.x -= 0.2f;
            std::println("{} - {} - {}", eye.x, eye.y, eye.z);
        }

        Renderer::clearRenderer(eye);

        for(auto l : mapFile.value().lineDefs){
            glm::vec4 color{1.0f, 0.0f, 0.0f, 1.0f};
            
            if(l.flags & std::to_underlying(LineDefFormat::TWO_SIDE)){
                color.r = 0.0f;
                color.g = 1.0f;
            }
            Renderer::drawLine(reMapVertex.at(l.startIndex), reMapVertex.at(l.endIndex), 5.0f, color);
        }

        for(auto v : reMapVertex){
            Renderer::drawPoint(v, 5.0f, {1.0f, 1.0f, 1.0f, 1.0f});
        }

        // Renderer::drawPoint({100.0f, 10.0f}, 50.0f, {1.0f, 0.0f, 0.0f, 1.0f});

        // Renderer::drawQuad({300.0f, 200.0f}, {100.0f, 100.0f}, 45.0f, {1.0f, 1.0f, 1.0f, 1.0f});

        // Renderer::drawLine({0.0f, 0.0f}, {width, height}, 4.0f, {1.0f, 1.0f, 0.0f, 1.0f});
        
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}
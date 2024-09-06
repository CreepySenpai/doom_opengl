#include <print>

#include <Creepy/Engine.hpp>
#include <Creepy/WAD.hpp>
#include <Creepy/Camera.hpp>
#include <Creepy/Renderer.hpp>
#include <Creepy/Input.hpp>

constexpr float fov{60.0f};
Camera s_camera{};
glm::vec2 s_lastMousePosition{};

constexpr float playerSpeed{5.0f};
constexpr float mouseSensitivity{0.5f};

void Engine::Init(const WAD& wadFile, std::string_view mapName) {
    s_camera.Position = glm::vec3{0.0f, 0.0f, -3.0f};
    s_camera.Pitch = 0.0f;
    s_camera.Yaw = 0.0f;

    auto mapFile = WAD::readMap(mapName, wadFile).value();
    
    Renderer::setProjectionMatrix(glm::perspectiveLH(glm::radians(fov), 
        static_cast<float>(Renderer::getSize().x) / static_cast<float>(Renderer::getSize().y), 0.001f, 100.0f));
}

void Engine::Update(float deltaTime) {
    Camera::UpdateDirection(s_camera);

    if(Input::IsKeyPressed(KeyCode::KEY_UP)){
        s_camera.Position += s_camera.Forward * playerSpeed * deltaTime;
    }

    if(Input::IsKeyPressed(KeyCode::KEY_DOWN)){
        s_camera.Position -= s_camera.Forward * playerSpeed * deltaTime;
    }

    if(Input::IsKeyPressed(KeyCode::KEY_LEFT)){
        s_camera.Position -= s_camera.Right * playerSpeed * deltaTime;
    }

    if(Input::IsKeyPressed(KeyCode::KEY_RIGHT)){
        s_camera.Position += s_camera.Right * playerSpeed * deltaTime;
    }

    if(Input::IsKeyPressed(KeyCode::KEY_SPACE)){
        s_camera.Position += s_camera.Up * playerSpeed * deltaTime;
    }

    if(Input::IsKeyPressed(KeyCode::KEY_Z)){
        s_camera.Position -= s_camera.Up * playerSpeed * deltaTime;
    }

    if(Input::IsButtonPressed(ButtonCode::BUTTON_RIGHT)){
        if(!Input::IsMouseCapture()){
            s_lastMousePosition = Input::GetMousePosition();
            Input::SetMouseCapture(1);
        }

        const auto currentMousePosition = Input::GetMousePosition();
        const auto deltaMouse = currentMousePosition - s_lastMousePosition;
        s_lastMousePosition = currentMousePosition;

        s_camera.Yaw += deltaMouse.x * mouseSensitivity * deltaTime;
        s_camera.Pitch += deltaMouse.y * mouseSensitivity * deltaTime;
        s_camera.Pitch = std::clamp(s_camera.Pitch, -glm::radians(90.0f), glm::radians(90.0f));
    }
    else if(Input::IsMouseCapture()){
        Input::SetMouseCapture(0);
    }
}

void Engine::Render(){

    Renderer::setViewMatrix(glm::lookAtLH(s_camera.Position, s_camera.Position + s_camera.Forward, s_camera.Up));


    Renderer::drawQuad({0.0f, 0.0f}, {30.0f, 40.0f}, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f});
}
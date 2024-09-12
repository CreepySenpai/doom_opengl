#include <print>
#include <utility>
#include <random>

#include <Creepy/Engine.hpp>
#include <Creepy/WAD.hpp>
#include <Creepy/Camera.hpp>
#include <Creepy/Renderer.hpp>
#include <Creepy/Input.hpp>
#include <Creepy/Mesh.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

constexpr float fov{60.0f};
Camera s_camera{};
glm::vec2 s_lastMousePosition{};

constexpr float playerSpeed{5.0f};
constexpr float mouseSensitivity{0.5f};

struct WallNode{
    glm::mat4 Model;
    glm::vec4 Color;
    const Sector* SectorPtr;
};

struct FlatNode{
    glm::mat4 Model;
    glm::vec4 Color;
    const Sector* SectorPtr;
};

std::vector<WallNode> s_wallNodeLists;
std::vector<FlatNode> s_flatNodeLists;

float modelAngle{0.0f};

static glm::mat4 verticesToModel(glm::vec3 point0, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);

void Engine::Init(const WAD& wadFile, std::string_view mapName) {
    s_camera.Position = glm::vec3{0.0f, 0.0f, -30.0f};
    // s_camera.Position = glm::vec3{0.0f, 0.0f, -3.0f};
    s_camera.Pitch = 0.0f;
    s_camera.Yaw = 0.0f;

    auto mapFile = WAD::readMap(mapName, wadFile).value();

    std::string glMapName{std::format("GL_{}", mapName)};
    auto glMapFile = WAD::readGLMap(glMapName, wadFile).value();
    
    Renderer::setProjectionMatrix(glm::perspectiveLH(glm::radians(fov), 
        static_cast<float>(Renderer::getSize().x) / static_cast<float>(Renderer::getSize().y), 0.001f, 100.0f));

    s_wallNodeLists.reserve(mapFile.lineDefs.size());

    for(auto&& subSec : glMapFile.subSectors){
        const auto numVertex = subSec.numSegments;
        std::vector<Vertex> vertices;
        vertices.reserve(numVertex);
        // (1 << 15)
        for(uint16_t i{}; i < numVertex; ++i){
            auto&& segment = glMapFile.segments.at(i + subSec.firstSegment);

        }
    }
    
    for(auto&& line : mapFile.lineDefs){
        if(line.flags & std::to_underlying(LineDefFormat::TWO_SIDE)){
            const auto start = mapFile.vertices.at(line.startIndex);
            const auto end = mapFile.vertices.at(line.endIndex);

            auto&& frontSector = mapFile.sectors.at(mapFile.sideDefs.at(line.frontSideDef).sectorIndex);
            auto&& backSector = mapFile.sectors.at(mapFile.sideDefs.at(line.backSideDef).sectorIndex);

            {   // Floor Node
                const glm::vec3 floor_0{start.x, static_cast<float>(frontSector.floor), start.y};
                const glm::vec3 floor_1{end.x, static_cast<float>(frontSector.floor), end.y};
                const glm::vec3 floor_2{end.x, static_cast<float>(backSector.floor), end.y};
                const glm::vec3 floor_3{start.x, static_cast<float>(backSector.floor), start.y};

                s_wallNodeLists.push_back({verticesToModel(floor_0, floor_1, floor_2, floor_3), 
                    {1.0f, 1.0f, 1.0f, 1.0f}, 
                    &mapFile.sectors.at(mapFile.sideDefs.at(line.frontSideDef).sectorIndex)});
            }

            {   // Ceiling Node
                const glm::vec3 ceiling_0{start.x, static_cast<float>(frontSector.ceiling), start.y};
                const glm::vec3 ceiling_1{end.x, static_cast<float>(frontSector.ceiling), end.y};
                const glm::vec3 ceiling_2{end.x, static_cast<float>(backSector.ceiling), end.y};
                const glm::vec3 ceiling_3{start.x, static_cast<float>(backSector.ceiling), start.y};

                s_wallNodeLists.push_back({verticesToModel(ceiling_0, ceiling_1, ceiling_2, ceiling_3), 
                    {1.0f, 1.0f, 1.0f, 1.0f}, 
                    &mapFile.sectors.at(mapFile.sideDefs.at(line.frontSideDef).sectorIndex)});
            }
        }
        else {
            constexpr float scaleFactor{100.0f};
            const auto start = mapFile.vertices.at(line.startIndex) / scaleFactor;
            const auto end = mapFile.vertices.at(line.endIndex) / scaleFactor;

            auto&& frontSector = mapFile.sectors.at(mapFile.sideDefs.at(line.frontSideDef).sectorIndex);

            const float secFloor = static_cast<float>(frontSector.floor) / scaleFactor;
            const float secCeiling = static_cast<float>(frontSector.ceiling) / scaleFactor;
            
            const float x = end.x - start.x;
            const float y = end.y - start.y;
            const float length = std::sqrt(x * x + y * y);
            const float height = secCeiling - secFloor;

            const float angle = std::atan2(y, x) * -1.0f;   // Flip Rotation Angle
            const auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3{start.x, secFloor, start.y});
            const auto scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{length, height, 1.0f});
            const auto rotationMatrix = glm::rotate(glm::identity<glm::mat4>(), angle, {0.0f, 1.0f, 0.0f});
            s_wallNodeLists.push_back({translationMatrix * rotationMatrix * scaleMatrix, {1.0f, 1.0f, 1.0f, 1.0f}, &mapFile.sectors.at(mapFile.sideDefs.at(line.frontSideDef).sectorIndex)});
        }
    }
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

    if(Input::IsKeyPressed(KeyCode::KEY_N)){
        modelAngle += 0.1f;
        std::println("Angle: {}", glm::radians(modelAngle));
    }

    if(Input::IsKeyPressed(KeyCode::KEY_M)){
        modelAngle -= 0.1f;
        std::println("Angle: {}", glm::radians(modelAngle));
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

static glm::vec4 getRandomColor(const uint64_t seed);

extern Mesh s_quadMesh;

void Engine::Render(){
    Renderer::setViewMatrix(glm::lookAtLH(s_camera.Position, s_camera.Position + s_camera.Forward, s_camera.Up));

    // glm::mat4 tran = glm::translate(glm::identity<glm::mat4>(), {0.0f, 0.0f, 0.0f});
    // glm::mat4 scal = glm::scale(glm::identity<glm::mat4>(), {1.0f, 1.0f, 1.0f});
    
    // glm::mat4 rott = glm::rotate(glm::identity<glm::mat4>(), glm::radians(modelAngle), glm::vec3{0.0f, 1.0f, 0.0f});
    // Renderer::drawMesh(s_quadMesh, tran * rott * scal, {1.0f, 1.0f, 1.0f, 1.0f});
    for(const auto& node : s_wallNodeLists){
        auto color = getRandomColor((uint64_t)node.SectorPtr) * (static_cast<float>(node.SectorPtr->lightLevel) / 255.0f);
        Renderer::drawMesh(s_quadMesh, node.Model, color);
    }
}

// We use ptr to gen color
glm::vec4 getRandomColor(const uint64_t seed){
    std::mt19937_64 gen{seed};
    std::uniform_real_distribution<float> dis{0.0f, 1.0f};
    return glm::vec4{dis(gen), dis(gen), dis(gen), 1.0f};
}

glm::mat4 verticesToModel(glm::vec3 point0, glm::vec3 point1, glm::vec3 point2, glm::vec3 point3) {
    constexpr float scaleFactor{100.0f};
    point0 /= scaleFactor;
    point1 /= scaleFactor;
    point2 /= scaleFactor;
    point3 /= scaleFactor;

    const float x = point1.x - point0.x;
    const float y = point1.z - point0.z;
    const float length = std::sqrt(x * x + y * y);
    const float height = point3.y - point0.y;

    const float angle = std::atan2(y, x) * -1.0f;   // Flip Rotation Angle
    const auto translationMatrix = glm::translate(glm::identity<glm::mat4>(), point0);
    const auto scaleMatrix = glm::scale(glm::identity<glm::mat4>(), glm::vec3{length, height, 1.0f});
    const auto rotationMatrix = glm::rotate(glm::identity<glm::mat4>(), angle, {0.0f, 1.0f, 0.0f});

    return translationMatrix * rotationMatrix * scaleMatrix;
}
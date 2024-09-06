#include <Creepy/Camera.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

static constexpr glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};
static constexpr glm::vec3 WorldForward{0.0f, 0.0f, 1.0f};

void Camera::UpdateDirection(Camera& camera){

    const glm::quat quaternion{glm::vec3{camera.Pitch, camera.Yaw, 0.0f}};
    
    camera.Forward = glm::normalize(glm::rotate(quaternion, WorldForward));
    camera.Right = glm::normalize(glm::cross(WorldUp, camera.Forward));

    camera.Up = glm::normalize(glm::cross(camera.Forward, camera.Right));
    
}
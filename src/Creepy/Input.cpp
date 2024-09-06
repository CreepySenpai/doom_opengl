#include <bitset>
#include <utility>
#include <Creepy/Input.hpp>

static GLFWwindow* s_window{nullptr};
static glm::vec2 s_mousePosition{};

static std::bitset<std::to_underlying(KeyCode::KEY_MENU) + 1> s_keyCodeStates;
static std::bitset<std::to_underlying(ButtonCode::BUTTON_MIDDLE) + 1> s_buttonCodeStates;

void Input::Init(GLFWwindow* window){
    s_window = window;

    glfwSetKeyCallback(s_window, [](GLFWwindow* window, int keyCode, int scanCode, int action, int mods){
        if(keyCode < 0){
            return;
        }

        if(action == GLFW_PRESS){
            s_keyCodeStates[keyCode] = true;
        }
        else if(action == GLFW_RELEASE){
            s_keyCodeStates[keyCode] = false;
        }
        
    });

    glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int buttonCode, int action, int mods){
        if(buttonCode < 0){
            return;
        }

        if(action == GLFW_PRESS){
            s_buttonCodeStates[buttonCode] = true;
        }
        else if(action == GLFW_RELEASE){
            s_buttonCodeStates[buttonCode] = false;
        }
    });

    glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y){
        s_mousePosition.x = static_cast<float>(x);
        s_mousePosition.y = static_cast<float>(y);
    });
}

bool Input::IsKeyPressed(KeyCode keyCode) {
    return static_cast<bool>(s_keyCodeStates[std::to_underlying(keyCode)]);
}

bool Input::IsButtonPressed(ButtonCode buttonCode) {
    return static_cast<bool>(s_buttonCodeStates[std::to_underlying(buttonCode)]);
}

glm::vec2 Input::GetMousePosition() {
    return s_mousePosition;
}

bool Input::IsMouseCapture() {
    return glfwGetInputMode(s_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Input::SetMouseCapture(int isCapture) {
    glfwSetInputMode(s_window, GLFW_CURSOR, isCapture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
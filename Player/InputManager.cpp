#include "InputManager.h"
#include <glm/gtc/matrix_transform.hpp>

InputManager::InputManager()
    : window(nullptr), lastX(640), lastY(360), dragging(false), sensitivity(0.2f), speed(2.5f) {}

void InputManager::setWindow(GLFWwindow* window) {
    this->window = window;
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
}

void InputManager::updateCamera(float& yaw, float& pitch, glm::vec3& cameraFront) {
    if (dragging) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float xoffset = static_cast<float>(xpos - lastX);   
        float yoffset = static_cast<float>(lastY - ypos);

        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void InputManager::processInput(GLFWwindow* window, glm::vec3& cameraPos, const glm::vec3& cameraFront, const glm::vec3& cameraUp, float deltaTime) {
    float velocity = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
}

void InputManager::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (inputManager && inputManager->dragging) {  // 确保 inputManager 非空
        inputManager->lastX = xpos;
        inputManager->lastY = ypos;
    }
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (inputManager) {  // 确保 inputManager 非空
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                inputManager->dragging = true;
                glfwGetCursorPos(window, &inputManager->lastX, &inputManager->lastY);
            }
            else if (action == GLFW_RELEASE) {
                inputManager->dragging = false;
            }
        }
    }
}

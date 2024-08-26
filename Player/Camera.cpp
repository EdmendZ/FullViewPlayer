#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : yaw(-90.0f), pitch(0.0f), lastX(640), lastY(360), dragging(false), sensitivity(0.2f),
    cameraPos(glm::vec3(0.0f, 0.0f, 1.0f)), cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)) {}

void Camera::ProcessMouseMovement(double xpos, double ypos) {
    if (dragging) {
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos);  // Reversed since y-coordinates go from bottom to top

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

void Camera::StartDrag(GLFWwindow* window) {
    dragging = true;
    glfwGetCursorPos(window, &lastX, &lastY);
}

void Camera::StopDrag() {
    dragging = false;
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

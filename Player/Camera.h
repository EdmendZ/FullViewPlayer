#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera();
    void ProcessMouseMovement(double xpos, double ypos);
    void StartDrag(GLFWwindow* window);
    void StopDrag();
    glm::mat4 GetViewMatrix();
private:
    float yaw;
    float pitch;
    double lastX;
    double lastY;
    bool dragging;
    float sensitivity;
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
};

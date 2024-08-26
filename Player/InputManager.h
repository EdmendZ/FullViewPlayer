#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager {
public:
    InputManager();
    void setWindow(GLFWwindow* window);
    void updateCamera(float& yaw, float& pitch, glm::vec3& cameraFront);
    void processInput(GLFWwindow* window, glm::vec3& cameraPos, const glm::vec3& cameraFront, const glm::vec3& cameraUp, float deltaTime);

private:
    GLFWwindow* window;
    double lastX, lastY;
    bool dragging;
    float sensitivity;
    float speed;

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

#endif // INPUTMANAGER_H

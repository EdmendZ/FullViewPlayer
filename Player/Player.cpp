#include "OpenGLRenderer.h"
#include "VideoDecoder.h"
#include <glm/glm.hpp>

// Camera settings
float yaw = -90.0f;
float pitch = 0.0f;
glm::vec3 cameraPos(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float sensitivity = 0.2f;
double lastX = 640, lastY = 360;
bool dragging = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (dragging) {
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            dragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE) {
            dragging = false;
        }
    }
}

int main() {
    VideoDecoder decoder("demo4.mp4");
    if (!decoder.initialize()) {
        return -1;
    }

    // 输出视频格式信息
    decoder.printVideoInfo();

    OpenGLRenderer renderer;
    if (!renderer.initialize(1280, 720)) {
        return -1;
    }

    renderer.loadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    renderer.createSphere(5.0f, 50, 50);

    GLFWwindow* window = renderer.getWindow();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        if (decoder.decodeFrame()) {
            renderer.renderFrame(decoder.getFrameData(), decoder.getWidth(), decoder.getHeight());
            renderer.updateCameraView(yaw, pitch, cameraPos, cameraFront, cameraUp);
        }
        glfwPollEvents();
    }

    return 0;
}

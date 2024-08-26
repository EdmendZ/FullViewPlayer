#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class OpenGLRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    bool initialize(int width, int height);
    void loadShaders(const std::string& vertexPath, const std::string& fragmentPath);
    void createSphere(float radius, unsigned int rings, unsigned int sectors);
    void renderFrame(uint8_t* frameData, int frameWidth, int frameHeight);
    void updateCameraView(float yaw, float pitch, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);
    GLFWwindow* getWindow() const;

private:
    GLuint compileShader(GLenum type, const std::string& source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    std::string readFile(const std::string& filePath);

    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    GLuint textureID;
    GLuint modelLoc, viewLoc, projLoc;
    glm::mat4 model, projection;
};

#endif // OPENGLRENDERER_H

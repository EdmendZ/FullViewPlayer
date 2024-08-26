#include "OpenGLRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 (M_PI / 2.0)
#endif




OpenGLRenderer::OpenGLRenderer() : window(nullptr), shaderProgram(0), VAO(0), VBO(0), EBO(0), textureID(0) {}

OpenGLRenderer::~OpenGLRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool OpenGLRenderer::initialize(int width, int height) {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    window = glfwCreateWindow(width, height, "360 Video Player", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

void OpenGLRenderer::loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    shaderProgram = linkProgram(vertexShader, fragmentShader);
    glUseProgram(shaderProgram);

    modelLoc = glGetUniformLocation(shaderProgram, "model");
    viewLoc = glGetUniformLocation(shaderProgram, "view");
    projLoc = glGetUniformLocation(shaderProgram, "projection");

    model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)1280 / (float)720, 0.1f, 50.0f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void OpenGLRenderer::createSphere(float radius, unsigned int rings, unsigned int sectors) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    float const R = 1.0f / (static_cast<float>(rings) - 1);
    float const S = 1.0f / (static_cast<float>(sectors) - 1);
    int r, s;

    vertices.resize(rings * sectors * 5);
    std::vector<float>::iterator v = vertices.begin();
    for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
        float const y = sin(-M_PI_2 + M_PI * r * R);
        float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
        float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

        *v++ = x * radius;
        *v++ = y * radius;
        *v++ = z * radius;

        *v++ = s * S;
        *v++ = 1 - r * R;
    }

    indices.resize(rings * sectors * 6);
    std::vector<unsigned int>::iterator i = indices.begin();
    for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
        *i++ = r * sectors + s;
        *i++ = r * sectors + (s + 1);
        *i++ = (r + 1) * sectors + (s + 1);
        *i++ = (r + 1) * sectors + (s + 1);
        *i++ = (r + 1) * sectors + s;
        *i++ = r * sectors + s;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void OpenGLRenderer::renderFrame(uint8_t* frameData, int frameWidth, int frameHeight) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frameData);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6 * (50 - 1) * (50 - 1), GL_UNSIGNED_INT, 0);
    glDepthMask(GL_TRUE);

    glfwSwapBuffers(window);
}

void OpenGLRenderer::updateCameraView(float yaw, float pitch, glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp) {
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

GLFWwindow* OpenGLRenderer::getWindow() const {
    return window;
}

GLuint OpenGLRenderer::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint OpenGLRenderer::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program Linking Error:\n" << infoLog << std::endl;
    }
    return program;
}

std::string OpenGLRenderer::readFile(const std::string& filePath) {
    std::string code;
    std::ifstream file(filePath, std::ios::in);
    if (file.is_open()) {
        std::stringstream stream;
        stream << file.rdbuf();
        code = stream.str();
        file.close();
    }
    return code;
}

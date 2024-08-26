#pragma once
#include <GL/glew.h>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif


class Sphere {
public:
    Sphere(float radius, unsigned int rings, unsigned int sectors);
    GLuint getVAO();
    unsigned int getIndexCount();
private:
    GLuint VAO, VBO, EBO;
    unsigned int indexCount;
    void createSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int rings, unsigned int sectors);
};

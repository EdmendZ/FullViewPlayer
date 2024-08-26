#include "Sphere.h"
#include <GL/glew.h>


Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    createSphere(vertices, indices, radius, rings, sectors);

    indexCount = indices.size();

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
}

GLuint Sphere::getVAO() {
    return VAO;
}

unsigned int Sphere::getIndexCount() {
    return indexCount;
}

void Sphere::createSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int rings, unsigned int sectors) {
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
        *v++ = 1 - r * R;  // Flip the v coordinate to correct upside-down texture
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
}

#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Balloon.h" // Ensure the Balloon class is defined and included
#include "Vertex.h"  // Include Vertex structure

class Renderer {
public:
    Renderer();
    ~Renderer();

    std::vector<Vertex> createBalloonVertices(const Balloon& balloon);
    void initialize();
    void render(const std::vector<class Balloon>& balloons);
    void setProjectionMatrix(const glm::mat4& proj);
    void resize(int width, int height);
    void cleanup();

    GLuint programID;
    GLuint VAO, VBO;

private:
    
};

#endif

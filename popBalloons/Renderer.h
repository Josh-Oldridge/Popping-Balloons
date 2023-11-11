#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Balloon.h" // Ensure the Balloon class is defined and included
#include "Vertex.h"  // Include Vertex structure
#include "Fragment.h"

// This struct should be defined in Renderer.h or another relevant header file where it's used.

struct FragmentVertexData {
    glm::vec3 position; // Position in space
    glm::vec4 color;    // RGBA color with alpha

    FragmentVertexData(const glm::vec3& pos, const glm::vec4& col)
        : position(pos), color(col) {}
};
class Renderer {
public:
    Renderer();
    ~Renderer();

    std::vector<Vertex> createBalloonVertices(const Balloon& balloon);
    void initialize();
    void render(const std::vector<Balloon>& balloons, const std::vector<Fragment>& fragments);
    void setProjectionMatrix(const glm::mat4& proj);
    void resize(int width, int height);
    void cleanup();

    //GLuint programID;
    //GLuint VAO, VBO;

private:
    
    glm::mat4 projectionMatrix;

    GLuint balloonProgramID;
    GLuint balloonVAO;
    GLuint balloonVBO;

    
    GLuint fragmentVAO;
    GLuint fragmentVBO;
};

#endif

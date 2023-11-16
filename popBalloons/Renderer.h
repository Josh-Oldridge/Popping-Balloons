#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Balloon.h" 
#include "Vertex.h"  
#include "Fragment.h"


struct FragmentVertexData {
    glm::vec3 position; // Position in space
    glm::vec4 color;    // RGBA color with alpha
    float size;         // Size of the fragment

    FragmentVertexData(const glm::vec3& pos, const glm::vec4& col, float s)
        : position(pos), color(col), size(s) {}
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
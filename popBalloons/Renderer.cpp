#include "Renderer.h"
#include <common/shader.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <iostream>



Renderer::Renderer() {
    // Constructor will initialize OpenGL objects
}

Renderer::~Renderer() {
    cleanup(); // Cleanup will deallocate OpenGL objects
}

void Renderer::initialize() {
    // Create and compile the GLSL program from the shaders
    balloonProgramID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    // Balloon VAO and VBO setup
    glGenVertexArrays(1, &balloonVAO);
    glBindVertexArray(balloonVAO);

    glGenBuffers(1, &balloonVBO);
    glBindBuffer(GL_ARRAY_BUFFER, balloonVBO);

    // Define the vertex data layout for balloons
    glEnableVertexAttribArray(0); // for vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1); // for vertex colors
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

    glBindVertexArray(0); // Unbind the VAO

    // Fragment particles VAO and VBO setup
    glGenVertexArrays(1, &fragmentVAO);
    glBindVertexArray(fragmentVAO);

    glGenBuffers(1, &fragmentVBO);
    glBindBuffer(GL_ARRAY_BUFFER, fragmentVBO);

    // Define the vertex data layout for fragment particles
    glEnableVertexAttribArray(0); // for fragment positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FragmentVertexData), (void*)0);

    glEnableVertexAttribArray(1); // for fragment colors
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FragmentVertexData), (void*)(offsetof(FragmentVertexData, color)));

    glBindVertexArray(0); // Unbind the VAO
}

void Renderer::setProjectionMatrix(const glm::mat4& proj) {
    projectionMatrix = proj;

    // Assuming you're using a uniform to set the projection matrix in the shader
    glUseProgram(balloonProgramID); // or any other program ID that needs the projection matrix
    GLuint matrixID = glGetUniformLocation(balloonProgramID, "MVP"); // Change "MVP" if the uniform has another name
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void Renderer::render(const std::vector<Balloon>& balloons, const std::vector<Fragment>& fragments) {
    // Use the same shader program for both balloons and fragments
    glUseProgram(balloonProgramID); // Use the shader program
    glBindVertexArray(balloonVAO);
    for (const Balloon& balloon : balloons) {
        std::vector<Vertex> vertices = createBalloonVertices(balloon);
        
        glBindBuffer(GL_ARRAY_BUFFER, balloonVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
    }
    
    // Render fragments - if any
    if (!fragments.empty()) {
        glBindVertexArray(fragmentVAO);
        std::vector<FragmentVertexData> fragmentVertices;
        for (const Fragment& fragment : fragments) {
            fragmentVertices.emplace_back(fragment.position, fragment.color);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, fragmentVBO);
        glBufferData(GL_ARRAY_BUFFER, fragmentVertices.size() * sizeof(FragmentVertexData), fragmentVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, fragmentVertices.size());
    }

    // Unbind VAO to be safe
    glBindVertexArray(0);
}
void Renderer::resize(int width, int height) {
    if (width == 0 || height == 0) {
        std::cout << "Resize was called with a zero width or height, ignoring." << std::endl;
        return;
    }

    glViewport(0, 0, width, height);

    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);

    // Assuming that you only have one shader program for both balloons and fragments
    glUseProgram(balloonProgramID);
    GLuint matrixID = glGetUniformLocation(balloonProgramID, "MVP");
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(projection));

    std::cout << "Framebuffer size after resize: " << width << "x" << height << std::endl;
}

void Renderer::cleanup() {
    if (balloonVAO) {
        glDeleteVertexArrays(1, &balloonVAO);
        balloonVAO = 0;
    }
    if (balloonVBO) {
        glDeleteBuffers(1, &balloonVBO);
        balloonVBO = 0;
    }
    if (fragmentVAO) {
        glDeleteVertexArrays(1, &fragmentVAO);
        fragmentVAO = 0;
    }
    if (fragmentVBO) {
        glDeleteBuffers(1, &fragmentVBO);
        fragmentVBO = 0;
    }
    // Assuming that you only have one shader program for both balloons and fragments
    if (balloonProgramID) {
        glDeleteProgram(balloonProgramID);
        balloonProgramID = 0;
    }
}
std::vector<Vertex> Renderer::createBalloonVertices(const Balloon& balloon) {
    std::vector<Vertex> vertices;
    glm::vec3 position = balloon.getPosition(); // Balloon position
    glm::vec3 color = balloon.getColor(); // Balloon color
    float alphaValue = 1.0f; // Assuming a fully opaque balloon
    float radius = balloon.getSize(); // set the radius
    unsigned int num_segments = 20;  // decide the number of segments you want to divide your balloon into

// Center vertex
    vertices.emplace_back(Vertex{position.x, position.y, color.r, color.g, color.b, alphaValue});

// Perimeter vertices
    for (unsigned int i = 0; i <= num_segments; ++i) { // Note: Starts at 0, loops to <= num_segments
        float theta = 2.0f * glm::pi<float>() * float(i) / float(num_segments);
        float px = position.x + radius * cosf(theta); // compute x coordinate
        float py = position.y + radius * sinf(theta); // compute y coordinate

        vertices.emplace_back(Vertex{px, py, color.r, color.g, color.b, alphaValue});
    }
    // The loop must go from 0 to <= num_segments to close the loop correctly.

    return vertices;
}






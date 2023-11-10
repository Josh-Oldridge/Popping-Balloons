#include "Renderer.h"
#include <common/shader.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 



Renderer::Renderer() {
    // Constructor will initialize OpenGL objects
}

Renderer::~Renderer() {
    cleanup(); // Cleanup will deallocate OpenGL objects
}

void Renderer::initialize() {
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    // Vertex Array Object and Vertex Buffer Object for balloons are initialized here
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Define the vertex data layout
    glEnableVertexAttribArray(0); // for vertex positions
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1); // for vertex colors
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));

    glBindVertexArray(0); // Unbind the VAO
}

void Renderer::setProjectionMatrix(const glm::mat4& proj) {
    // Assuming 'proj' is the orthographic projection matrix
    if (programID) {
        GLuint matrixID = glGetUniformLocation(programID, "uProjectionMatrix");
        glUseProgram(programID);
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(proj));
    }
}

void Renderer::render(const std::vector<Balloon>& balloons) {
    // For each balloon in the game...
    for (const Balloon& balloon : balloons) {
        // Create vertices for the balloon based on its size, position, and color
        std::vector<Vertex> vertices = createBalloonVertices(balloon);

        // Update VBO data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        // Bind the vertex array object
        glBindVertexArray(VAO);

        // Draw the balloon
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());

        // Unbind the vertex array object
        glBindVertexArray(0);
    }
}

void Renderer::resize(int width, int height) {
    // Update the viewport
    glViewport(0, 0, width, height);

    // Calculate new aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Update the projection matrix for the new window size
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);

    // Update the shader's projection matrix uniform
    if (programID) {
        GLuint matrixID = glGetUniformLocation(programID, "uProjectionMatrix");
        glUseProgram(programID);
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(projection));
    }
}

void Renderer::cleanup() {
    // Cleanup VBO, VAO, and shader program
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(programID);
}

std::vector<Vertex> Renderer::createBalloonVertices(const Balloon& balloon) {
    std::vector<Vertex> vertices;
    glm::vec3 position = balloon.getPosition(); // Balloon position
    glm::vec3 color = balloon.getColor(); // Balloon color
    float alphaValue = 1.0f; // Assuming a fully opaque balloon
    float horizontal_radius = balloon.getSize() * 0.5f;
    float vertical_radius = balloon.getSize();
    unsigned int num_segments = 20;

    // Center vertex
    vertices.emplace_back(Vertex{position.x, position.y, color.r, color.g, color.b, alphaValue});

    // Perimeter vertices
    for (unsigned int i = 0; i <= num_segments; ++i) { // Note: Starts at 0, loops to <= num_segments
        float theta = 2.0f * glm::pi<float>() * float(i) / float(num_segments);
        float px = position.x + horizontal_radius * cosf(theta); // compute x coordinate
        float py = position.y + vertical_radius * sinf(theta); // compute y coordinate

        vertices.emplace_back(Vertex{px, py, color.r, color.g, color.b, alphaValue});
    }
    // The loop must go from 0 to <= num_segments to close the loop correctly.

    return vertices;
}






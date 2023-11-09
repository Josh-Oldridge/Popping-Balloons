// popBalloons.cpp
#include "popBalloons.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

#include <vector>

// Define a structure for Vertex data

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// Initialize a shader program
GLuint programID;
// Initialize Vertex Array Object and Vertex Buffer Object
GLuint VAO, VBO;

PopBalloons::PopBalloons() {
    // Constructor logic, set up any initial values
}

PopBalloons::~PopBalloons() {
    // Destructor logic, clean up resources
}

void PopBalloons::Run() {
    // Initialize GLFW, GLEW, create window, etc.
    if (!initializeGLFW() || !initializeWindow() || !initializeGLEW()) {
        std::cerr << "Initialization failed." << std::endl;
        return;
    }

    setupScene();

    // This is your main loop
    while (!glfwWindowShouldClose(window)) {
        // Update the scene
        updateScene();

        // Render the scene
        renderScene();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    cleanup();
}

bool PopBalloons::initializeGLFW() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    return true;
}

bool PopBalloons::initializeWindow() {
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Pop Balloons", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    return true;
}

bool PopBalloons::initializeGLEW() {
    // Initialize GLEW
    glewExperimental = GL_TRUE; 
    if (GLEW_OK != glewInit()) {
        std::cerr << "Failed to initialize GLEW\n";
        return false;
    }

    // Define the viewport dimensions
    glViewport(0, 0, 640, 480);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it's closer to the camera than the former one
    glDepthFunc(GL_LESS);

    return true;
}

void PopBalloons::setupScene() {
    //Create and compile our GLSL program from the shaders
    programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );

    // Define vertices for a filled circle (balloon)
    std::vector<Vertex> vertices;
    float horizontal_radius = 0.2f; // Horizontal radius of the balloon
    float vertical_radius = 0.3f;   // Vertical radius of the balloon, make this larger than horizontal_radius
    unsigned int num_segments = 32; // Increase for a smoother circle

    // Add the center vertex for the triangle fan
    vertices.push_back({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)}); // Center vertex, red color

    // Loop to create vertices around the center vertex
    for (unsigned int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Current angle

        Vertex v;

        // Calculate position for vertices to create an upright oval
        v.position.x = horizontal_radius * cosf(theta); // Use horizontal radius for x-axis
        v.position.y = vertical_radius * sinf(theta);   // Use vertical radius for y-axis
        v.position.z = 0.0f;

        // Set color to red
        v.color.r = 1.0f;
        v.color.g = 0.0f;
        v.color.b = 0.0f;

        vertices.push_back(v);
    }
    

    // Create Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Give our vertices to OpenGL
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,              //attribute. No particular reason for 0, but must match the layout in the shader
        3,              //size of the vertex arrribute
        GL_FLOAT,       //type
        GL_FALSE,       //normalized
        sizeof(Vertex), //stride
        (void*)0        //array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                              // attribute. Matches layout in shader
        3,                              // size
        GL_FLOAT,                       // type
        GL_FALSE,                       // normalized
        sizeof(Vertex),                 // stride
        (void*)offsetof(Vertex, color)  // array buffer offset
    );

    // Unbind the VAO for now to avoid accidental modification
    glBindVertexArray(0);
}

void PopBalloons::updateScene() {
    // Update your scene (move balloons, check for popping, etc.)
}





void PopBalloons::renderScene() {
    
    unsigned int num_segments = 32;
   
    // Set clear color to something other than black, for visibility
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // dark gray background for visibility
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // Use our shader
    glUseProgram(programID);

    // Bind the VAO
    glBindVertexArray(VAO);

   
    glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments + 2); 

    

    // Unbind the VAO
    glBindVertexArray(0);

}
void PopBalloons::cleanup() {
    // Cleanup VBO and shader
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(programID);
}

// Add definitions for any additional member functions here

// The main function could look like this:
int main() {
    PopBalloons game;
    game.Run();
    return 0;
}
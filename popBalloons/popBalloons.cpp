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
#include <random>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp> // for glm::ortho


// Define a structure for Vertex data


// Initialize a shader program
GLuint programID;
// Initialize Vertex Array Object and Vertex Buffer Object
GLuint VAO, VBO;

PopBalloons::PopBalloons()
    : lastTime(glfwGetTime()), // Initialize lastTime to current time
      nextBalloonTime(0), 
      gen(std::random_device{}()), 
      score(0), 
      lives(3) 
{
    nextBalloonTime = lastTime + generateRandomTime(); // Set initial timer using lastTime
    // ... other initialization code ...
}

PopBalloons::~PopBalloons() {
    // Destructor logic, clean up resources
}

// Inside PopBalloons class in popBalloons.cpp
float PopBalloons::generateRandomTime() {
    std::uniform_real_distribution<float> disTime(2.0f, 5.0f); // Between 2 and 5 seconds
    return disTime(gen); // Assuming gen is a std::mt19937 already defined
}

// Method to handle balloon popping
void PopBalloons::popBalloon(int balloonIndex) {
    Balloon& balloon = balloons[balloonIndex];
    // Calculate score based on size (smaller balloon gives higher score)
    score += static_cast<int>((0.15f - balloon.getSize()) * 100.0f);
    // Remove balloon from the vector
    balloons.erase(balloons.begin() + balloonIndex);
}

void PopBalloons::Run() {
    // Initialize GLFW, GLEW, create window, etc.
    if (!initializeGLFW() || !initializeWindow() || !initializeGLEW()) {
        std::cerr << "Initialization failed." << std::endl;
        return;
    }

    setupScene();

    // Initialize lastTime to the current time
    lastTime = glfwGetTime();

    // This is your main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate the time elapsed since the last frame
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime; // Update lastTime for the next frame

        // Update the scene with the calculated deltaTime
        updateScene(deltaTime);

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

    // Get the framebuffer size instead of window size in case of high-DPI screens
    
    
    return true;
}

bool PopBalloons::initializeWindow() {
    // Create a windowed mode window and its OpenGL context
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = glfwCreateWindow(mode->width, mode->height, "Pop Balloons", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);


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
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    // Get the framebuffer size
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
    glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f);

    // Pass the orthographic projection matrix to the shader
    GLuint matrixID = glGetUniformLocation(programID, "uProjectionMatrix");
    glUseProgram(programID);
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &projection[0][0]);

    // Define vertices for a filled circle (balloon)
    std::vector<Vertex> vertices;
    float horizontal_radius = 0.2f; // Horizontal radius of the balloon
    float vertical_radius = 0.3f;   // Vertical radius of the balloon, make this larger than horizontal_radius
    unsigned int num_segments = 32; // Increase for a smoother circle

    // Add the center vertex for the triangle fan
    vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);

    // Loop to create vertices around the center vertex
    for (unsigned int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Current angle
        float px = horizontal_radius * cosf(theta);
        float py = vertical_radius * sinf(theta);
        vertices.emplace_back(glm::vec3(px, py, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
    }
    

    // Create Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create Vertex Buffer Object
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Give our vertices to OpenGL
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 1st attribute buffer : vertices (2D positions here)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, r)));


    // Unbind the VAO
    glBindVertexArray(0);
}

void Balloon::update(float deltaTime) {
    position.y += 5 * deltaTime; // Adjust `movementSpeed` based on how fast you want balloons to move
}

void PopBalloons::updateScene(float deltaTime) {
    // Update each balloon using deltaTime
    for (auto& balloon : balloons) {
        balloon.update(deltaTime);
    }

    // Check if any balloons are off-screen and remove them
    for (size_t i = 0; i < balloons.size();) {
        if (balloons[i].isOffScreen()) {
            balloons.erase(balloons.begin() + i);
            --lives;
            // Game over logic
            if (lives <= 0) {
                endGame();
                return; // Stop the update loop because the game is over
            }
        } else {
            ++i;
        }
    }

    // Check if it's time to create a new balloon
    float currentTime = glfwGetTime();
    if (currentTime >= nextBalloonTime) {
        createBalloon(); // Create a new balloon at random position and color
        nextBalloonTime = currentTime + generateRandomTime(); // Set the time for creating the next balloon
    }
}

void PopBalloons::endGame() {
    std::cout << "Game Over! Your score: " << score << std::endl;
    // Perform any additional cleanup and exit the game
    // For example:
    glfwSetWindowShouldClose(window, 1); // Signal to close the game window
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

    for (const auto& balloon : balloons) {
        std::vector<Vertex> vertices = createBalloonVertices(balloon);

        // Update VBO data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);

        // Draw the balloon
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
    }

    glBindVertexArray(0);
}
void PopBalloons::createBalloon() {
    static std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution<float> dis(-1.0, 1.0); // For x-coordinate
    std::uniform_real_distribution<float> disColor(0.0, 1.0); // For color
    std::uniform_real_distribution<float> disSize(0.05f, 0.15f); // For size

    glm::vec3 position(dis(gen), -1.0f, 0.0f); // Start from the bottom of the screen
    glm::vec3 color(disColor(gen), disColor(gen), disColor(gen)); // Random color for each balloon
    float size = disSize(gen); // Random size for each balloon

    balloons.push_back(Balloon(position, size, color));
}

std::vector<Vertex> PopBalloons::createBalloonVertices(const Balloon& balloon) {
    std::vector<Vertex> vertices;
    
    // Retrieve position and color from the Balloon object
    glm::vec3 position = balloon.getPosition(); // Get the balloon position
    glm::vec3 color = balloon.getColor(); // Get the balloon color
    
    // Constants to define the oval shape and number of segments
    float horizontal_radius = balloon.getSize() * 0.5f; // Example horizontal size
    float vertical_radius = balloon.getSize(); // Example vertical size
    unsigned int num_segments = 20; // Increase for smoother circle (balloon)
    
    // Center vertex for triangle fan (we won't need a Z-coordinate for 2D rendering)
    vertices.emplace_back(position, color, 1.0f);
    
    // Loop to create vertices around the center
    for (unsigned int i = 0; i <= num_segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Current angle
        
        // Calculate position for vertices to create an upright oval
        float px = position.x + horizontal_radius * cosf(theta);
        float py = position.y + vertical_radius * sinf(theta);
        
        vertices.emplace_back(glm::vec3(px, py, 0.0f), color, 1.0f);
    }
    
    return vertices;
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
// popBalloons.h

#ifndef POPBALLOONS_H
#define POPBALLOONS_H

// Include standard libraries for graphics
#include <GL/glew.h>
#include <glm/glm.hpp>
// Include GLFW
#include <glfw3.h>
#include <vector>
#include <random>

// Define any global variables, constants, and structs for your balloon objects here

struct Vertex {
    float x, y;  // Position in 2D space
    float r, g, b, a;  // Color in RGBA format

    Vertex(glm::vec3 pos, glm::vec3 col, float alpha)
        : x(pos.x), y(pos.y), r(col.r), g(col.g), b(col.b), a(alpha) {}
};

class Balloon {
private:
    glm::vec3 position;
    float size;
    glm::vec3 color;

public:
    // Constructor
    Balloon(const glm::vec3& pos, float sz, const glm::vec3& col)
        : position(pos), size(sz), color(col) {}

    // Getter methods
    glm::vec3 getPosition() const { return position; }
    float getSize() const { return size; }
    glm::vec3 getColor() const { return color; }

    void update(float deltaTime);

    // Check if the balloon is off the screen
    bool isOffScreen() const {
        return position.y > 1.0f;
    }
};

class PopBalloons {
public:
    PopBalloons(); // Constructor
    ~PopBalloons(); // Destructor
    
    void Run(); // Main function to run the game loop
    void popBalloon(int balloonIndex);
    void endGame();
    void updateScene(float deltaTime); // Add deltaTime parameter


private:
    int score;
    int lives;
    bool initializeGLFW();
    bool initializeWindow();
    bool initializeGLEW();
    void setupScene();
    void updateScene();
    void renderScene();
    void cleanup();
    std::vector<Balloon> balloons;
    void createBalloon();
    std::vector<Vertex> createBalloonVertices(const Balloon& balloon);
    float nextBalloonTime; // The time when the next balloon should be created
    float generateRandomTime();
    std::mt19937 gen; // The random number generator instance
    double lastTime; // Time of the last frame

    
    // Add your variables for the window, shaders, and buffers here
    GLFWwindow* window;
    GLuint programID;
    int fbWidth, fbHeight;
    // More variables like buffers, array objects, etc.

    // Add any other private member functions or variables needed
};



#endif // POPBALLOONS_H
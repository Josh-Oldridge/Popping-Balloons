// popBalloons.h

#ifndef POPBALLOONS_H
#define POPBALLOONS_H

// Include standard libraries for graphics
#include <GL/glew.h>
#include <glm/glm.hpp>
// Include GLFW
#include <glfw3.h>

// Define any global variables, constants, and structs for your balloon objects here

class PopBalloons {
public:
    PopBalloons(); // Constructor
    ~PopBalloons(); // Destructor
    
    void Run(); // Main function to run the game loop

private:
    bool initializeGLFW();
    bool initializeWindow();
    bool initializeGLEW();
    void setupScene();
    void updateScene();
    void renderScene();
    void cleanup();
    
    // Add your variables for the window, shaders, and buffers here
    GLFWwindow* window;
    GLuint programID;
    // More variables like buffers, array objects, etc.

    // Add any other private member functions or variables needed
};

#endif // POPBALLOONS_H

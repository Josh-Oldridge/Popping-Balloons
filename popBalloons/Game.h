#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Balloon.h"
#include <vector>
#include <random>

class Game {
public:
    Game();
    ~Game();

    void run();
    void update(float deltaTime);
    void popBalloon(int balloonIndex);
    void createBalloon();
    void cleanup();
    
private:
    Renderer renderer; // Assumes initialization in constructor or init method
    GLFWwindow* window;
    std::vector<Balloon> balloons;
    int score;
    int lives;
    double lastTime;
    double nextBalloonTime;
    std::mt19937 gen;
    int fbWidth, fbHeight;
    
    float generateRandomTime();
    bool initializeGLFW();
    bool initializeWindow();
    bool initializeGLEW();
    void setupScene();
    void updateScene(float deltaTime);
    void renderScene();
    void registerClickCallback(); // Add this line
    void handleClick(float xpos, float ypos); // And this line
    void endGame();
};

#endif // GAME_H
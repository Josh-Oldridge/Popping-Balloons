#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include "Balloon.h"
#include "Fragment.h"
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
    Renderer renderer; 
    GLFWwindow* window;
    std::vector<Balloon> balloons;
    std::vector<Fragment> fragments;
    int score;
    int lives;
    double lastTime;
    double nextBalloonTime;
    std::mt19937 gen;
    int fbWidth, fbHeight;
    
    
    bool initializeGLFW();
    bool initializeWindow();
    bool initializeGLEW();
    void setupScene();
    void updateScene(float deltaTime);
    void renderScene();
    void registerClickCallback(); 
    void handleClick(float xpos, float ypos); 
    void endGame();
    float balloonSpeedMultiplier; 
    float balloonSpawnInterval; 
    float balloonSpawnSpeedIncrease; 
};

#endif // GAME_H
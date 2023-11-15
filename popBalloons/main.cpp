#include <iostream>


int main_cpp = []() -> int {
    std::cout << "main.cpp top-level static initialization." << std::endl;
    return 0;
}();
#include "Game.h"

int main() {
    std::cout << "Starting popBalloons game..." << std::endl;

    // Create an instance of the Game class
    Game game;

    // Before running the game, print that the Game instance has been created successfully
    std::cout << "Game instance created, entering the game loop." << std::endl;

    // Run the game
    game.run();

    // Print that the game.run() has finished executing
    std::cout << "Exiting the game loop, game ended." << std::endl;

    return 0;
}
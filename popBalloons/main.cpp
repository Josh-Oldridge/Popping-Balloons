#include <iostream>


int main_cpp = []() -> int {
    std::cout << "main.cpp top-level static initialization." << std::endl;
    return 0;
}();
#include "Game.h"

int main() {
    std::cout << "Starting popBalloons game..." << std::endl;

    
    Game game;
    std::cout << "Game instance created, entering the game loop." << std::endl;
    game.run();
    std::cout << "Exiting the game loop, game ended." << std::endl;

    return 0;
}
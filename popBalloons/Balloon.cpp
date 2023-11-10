#include "Balloon.h"

// Constructor implementation for the Balloon class
Balloon::Balloon(const glm::vec3& initPosition, float initSize, const glm::vec3& initColor)
    : position(initPosition), size(initSize), color(initColor) {}

// Update the balloon position by moving it upwards
void Balloon::update(float deltaTime) {
    // Adjust `movementSpeed` based on how fast you want balloons to move
    const float movementSpeed = 0.5f; // You can decide on a proper speed
    position.y += movementSpeed * deltaTime;

    // Additional logic can be added here if needed
}

// Check whether the balloon is off the screen (i.e., past the top of the window)
bool Balloon::isOffScreen() const {
    const float screenTop = 1.0f; // Assuming that the top of the screen has a y-coordinate of 1.0
    return position.y > screenTop;
}

// Getters for the Balloon class
glm::vec3 Balloon::getPosition() const {
    return position;
}

glm::vec3 Balloon::getColor() const {
    return color;
}

float Balloon::getSize() const {
    return size;
}
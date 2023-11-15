#include "Balloon.h"
#include <iostream>

// Constructor implementation for the Balloon class
Balloon::Balloon(const glm::vec3& position, float size, const glm::vec3& color)
    : position(position), size(size), color(color), velocity(0.0f, 0.05f, 0.0f), speed(1.0f) {
    // Reduced the y-component of velocity to 0.05f which should be slower
}

void Balloon::update(float deltaTime) {
    std::cout << "Updating balloon with deltaTime: " << deltaTime <<
                 ", speed: " << speed <<
                 ", velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
                 
    position += velocity * deltaTime * speed; // Update the position using velocity and speed

    // Additional logic...
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
void Balloon::setVelocity(const glm::vec3& newVelocity) {
    velocity = newVelocity;
}

glm::vec3 Balloon::getVelocity() const {
    return velocity;
}

void Balloon::setSpeed(float newSpeed) {
    speed = newSpeed;
}

float Balloon::getSpeed() const {
    return speed;
}
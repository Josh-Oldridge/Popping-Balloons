#include "Balloon.h"
#include <iostream>


Balloon::Balloon(const glm::vec3& position, float size, const glm::vec3& color)
    : position(position), size(size), color(color), velocity(0.0f, 0.05f, 0.0f), speed(1.0f) {
   
}

void Balloon::update(float deltaTime) {
    std::cout << "Updating balloon with deltaTime: " << deltaTime <<
                 ", speed: " << speed <<
                 ", velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
                 
    position += velocity * deltaTime * speed; // Update the position using velocity and speed

   
}

// Check whether the balloon is off the screen (i.e., past the top of the window)
bool Balloon::isOffScreen() const {
    const float screenTop = 1.0f; 
    return position.y > screenTop;
}


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
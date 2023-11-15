#ifndef BALLOON_H
#define BALLOON_H

#include <glm/glm.hpp>

class Balloon {
public:
    Balloon(const glm::vec3& position, float size, const glm::vec3& color);

    void update(float deltaTime);
    bool isOffScreen() const;
    glm::vec3 getPosition() const;
    glm::vec3 getColor() const;
    float getSize() const;


    void setVelocity(const glm::vec3& newVelocity);
    glm::vec3 getVelocity() const;

    void setSpeed(float newSpeed);
    float getSpeed() const;

private:
    glm::vec3 position;
    glm::vec3 velocity; // Balloon's velocity
    float speed; // Balloon's speed multiplier
    float size;
    glm::vec3 color;
};

#endif 
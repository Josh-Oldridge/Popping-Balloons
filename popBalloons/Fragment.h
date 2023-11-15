// Fragment.h

#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <glm/glm.hpp>

struct Fragment {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color; // RGBA, A for alpha transparency
    float size;
    float lifetime;  // How long the fragment should be displayed

    Fragment(const glm::vec3& pos, const glm::vec3& vel, const glm::vec4& col, float s, float life)
        : position(pos), velocity(vel), color(col), size(s), lifetime(life) {}
    
    
    void update(float deltaTime) {
        position += velocity * deltaTime;
        
        lifetime -= deltaTime;
    }
};

#endif // FRAGMENT_H
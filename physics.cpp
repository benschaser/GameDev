#include "physics.h"

void Physics::update(double dt) {
    velocity += acceleration * dt;
    // velocity.x *= damping;

    velocity.x = std::clamp(velocity.x, -terminal_velocity, terminal_velocity);
    velocity.y = std::clamp(velocity.y, -terminal_velocity, terminal_velocity);

    position += velocity * dt;
}
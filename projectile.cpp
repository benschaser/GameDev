#include "projectile.h"
#include "engine.h"
#include <cmath>

void Projectile::update(Engine& engine, double dt) {
    Physics old = physics;
    physics.acceleration.y = 0.0;
    physics.update(dt);

    // Check for collisions
    Vec<double> future{physics.position.x, old.position.y};
    Vec<double> vx{physics.velocity.x, 0};
    engine.world->move_to(future, size, vx);

    // attempt to move in y
    Vec<double> vy{0, physics.velocity.y};
    future.y = physics.position.y;
    engine.world->move_to(future, size, vy);

    // update postion & velocity
    physics.position = future;
    physics.velocity = {vx.x, vy.y};

    // rotate arrow if it is still moving
    if (physics.velocity.x != 0 && physics.velocity.y != 0) {
        sprite.angle = 90 - std::atan2(physics.velocity.y, physics.velocity.x) * 180 / M_PI;
    }

    // collided
    if (physics.velocity.x == 0 || physics.velocity.y == 0) {
        physics.velocity = {0, 0};
        physics.acceleration = {0, 0};
    }

    if (physics.velocity.x != 0 && physics.velocity.y != 0) {
        combat.attack_damage = 0;
        elapsed += dt;
        if (elapsed >= lifetime) {
            combat.is_alive = false;
        }
    }
}
#include "enemy.h"
#include "enemytype.h"
#include "engine.h"

Enemy::Enemy(const Vec<double>& position, const Vec<int>& size, EnemyType& type)
    :last_edge_position{position}, size{size}, type{type} {
        physics.position = position;
        physics.acceleration = type.acceleration;
        combat.health = type.health;
        combat.max_health = type.health;
        combat.attack_damage = type.damage;
        sprite = type.animation.get_sprite();
    }

std::unique_ptr<Command> Enemy::update(Engine& engine, double dt) {
    Physics old = physics;
    physics.update(dt);
    physics.velocity.x *= 0.92;

    // Check for collisions
    // attempt to move in x
    Vec<double> future{physics.position.x, old.position.y};
    Vec<double> vx{physics.velocity.x, 0};
    engine.world->move_to(future, size, vx);

    // attempt to move in y
    Vec<double> vy{0, physics.velocity.y};
    future.y = physics.position.y;
    engine.world->move_to(future, size, vy);

    // update position and velocity
    if (vx.x > type.x_velocity_max && (physics.acceleration.x > 0)) {
        vx.x = type.x_velocity_max;
    }
    else if (vx.x < -type.x_velocity_max && (physics.acceleration.x < 0)) {
        vx.x = -type.x_velocity_max;
    }
    physics.position = future;
    physics.velocity = {vx.x, vy.y};

    if (combat.invincible) {
        type.elapsed_time += dt;
        if (type.elapsed_time >= type.cooldown) {
            combat.invincible = false;
            type.elapsed_time = 0;
            physics.acceleration = type.acceleration;
        }
        else {

        }
    }

    // check for collision with wall
    if (vx.x == 0 && physics.acceleration.x != 0) {
        type.animation.flip(-physics.acceleration.x < 0);
        last_edge_position = physics.position;
        return std::make_unique<Accelerate>(-physics.acceleration.x);
    }

    type.animation.flip(physics.acceleration.x < 0);
    type.animation.update(dt);
    sprite = type.animation.get_sprite();

    return nullptr;
}

std::unique_ptr<Command> Enemy::next_action(Engine& engine) {
    if (combat.invincible) {
        return hurting_behavior(engine, *this);
    }
    return type.behavior(engine, *this);
}
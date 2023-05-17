#include "projectile.h"
#include "engine.h"
#include <cmath>

void Projectile::update(Engine& engine, double dt) {
    Physics old = physics;
    physics.acceleration.y = 0.0;
    physics.update(dt);
    anim_sprite.update(dt);
    sprite = anim_sprite.get_sprite();

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

    if (physics.velocity.x == 0 || hit_enemy) {
        if (elapsed == 0 && !hit_enemy) {
            engine.audio.play_sound("impact");
            if (sprite.flip) {
                Vec<double> pos_copy{physics.position.x + 0.5, physics.position.y};
                physics.position = pos_copy;
            }
            else {
                Vec<double> pos_copy{physics.position.x - 0.5, physics.position.y};
                physics.position = pos_copy;
            }
        }
        else if (elapsed == 0 && hit_enemy) {
            engine.audio.play_sound("laser_enemy_impact");
            if (sprite.flip) {
                Vec<double> pos_copy{physics.position.x + 0.5, physics.position.y};
                physics.position = pos_copy;
            }
            else {
                Vec<double> pos_copy{physics.position.x - 0.5, physics.position.y};
                physics.position = pos_copy;
            }
        }
        
        wall_impact_sprite.update(dt);
        enemy_impact_sprite.update(dt);
        sprite = wall_impact_sprite.get_sprite();
        if (hit_enemy) {
            sprite = enemy_impact_sprite.get_sprite();
        }
        physics.velocity = {0, 0};
        physics.acceleration = {0, 0};
        
        if (elapsed >= lifetime) {
            combat.is_alive = false;
        }
        else {
            combat.attack_damage = 0;
        }
        elapsed += dt;
    }
}
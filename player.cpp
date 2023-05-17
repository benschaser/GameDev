#include "player.h"
#include "engine.h"
#include "combat.h"
#include <iostream>

Player::Player(Engine& engine, const Vec<double>& position, const Vec<int>& size)
    :size{size} {
        physics.position = position;
        physics.acceleration.y = gravity;
        combat.health = 20;
        combat.max_health = 20;
        combat.attack_damage = 2;
        standing = engine.graphics.get_animated_sprite("astronaut_standing", 0.1, false, false);
        standing_carrying_g = engine.graphics.get_animated_sprite("astronaut_standing_carrying_g", 0.1, false, false);
        standing_carrying_o = engine.graphics.get_animated_sprite("astronaut_standing_carrying_o", 0.1, false, false);
        standing_carrying_p = engine.graphics.get_animated_sprite("astronaut_standing_carrying_p", 0.1, false, false);

        jumping = engine.graphics.get_animated_sprite("astronaut_jumping", 0.1, false, false);
        jumping_carrying_g = engine.graphics.get_animated_sprite("astronaut_jumping_carrying_g", 0.1, false, false);
        jumping_carrying_o = engine.graphics.get_animated_sprite("astronaut_jumping_carrying_o", 0.1, false, false);
        jumping_carrying_p = engine.graphics.get_animated_sprite("astronaut_jumping_carrying_p", 0.1, false, false);
        
        running = engine.graphics.get_animated_sprite("astronaut_running", 0.05, false, false);
        running_carrying_g = engine.graphics.get_animated_sprite("astronaut_running_carrying_g", 0.05, false, false);
        running_carrying_o = engine.graphics.get_animated_sprite("astronaut_running_carrying_o", 0.05, false, false);
        running_carrying_p = engine.graphics.get_animated_sprite("astronaut_running_carrying_p", 0.05, false, false);

        falling = engine.graphics.get_animated_sprite("astronaut_falling", 0.1, false, false);
        grounding = engine.graphics.get_animated_sprite("astronaut_grounding", 0.04, false, false);
        dying = engine.graphics.get_animated_sprite("astronaut_dying", 0.1);
        dying.loop = false;
        
        sprite = standing.get_sprite();

        projectile.anim_sprite = engine.graphics.get_animated_sprite("laser_g", 0.04, true, false);
        projectile.wall_impact_sprite = engine.graphics.get_animated_sprite("laser_g_wall_impact", 0.04, false, false);
        projectile.enemy_impact_sprite = engine.graphics.get_animated_sprite("laser_g_enemy_impact", 0.04, false, false);
        projectile.combat.invincible = true;
        projectile.combat.attack_damage = 1;
        projectile.physics.acceleration.y = gravity;

        state = std::make_unique<Standing>();
        state->enter(*this, engine);
    }

std::unique_ptr<Command> Player::handle_input(const SDL_Event& event, Engine& engine) {
    auto new_state = state->handle_input(*this, event);
    if (new_state) {
        state->exit(*this, engine);
        state = std::move(new_state);
        state->enter(*this, engine);
    }

    auto next = std::move(next_command);
    next_command = nullptr;
    return next;
}

void Player::update(Engine& engine, double dt) {
    auto new_state = state->update(*this, engine, dt);
    if (new_state) {
        state->exit(*this, engine);
        state = std::move(new_state);
        state->enter(*this, engine);
    }
    
    if (next_command) {
        next_command->execute(*this, engine);
        next_command = nullptr;
    }
    combat.attack_damage = gun_level*3 + 1;
    if (gun_level != prev_gun_level) {
        if (gun_level == 2) {
            projectile.anim_sprite = engine.graphics.get_animated_sprite("laser_p", 0.04, true, false);
            projectile.wall_impact_sprite = engine.graphics.get_animated_sprite("laser_p_wall_impact", 0.04, false, false);
            projectile.enemy_impact_sprite = engine.graphics.get_animated_sprite("laser_p_enemy_impact", 0.04, false, false);
        }
        else if (gun_level == 1) {
            projectile.anim_sprite = engine.graphics.get_animated_sprite("laser_o", 0.04, true, false);
            projectile.wall_impact_sprite = engine.graphics.get_animated_sprite("laser_o_wall_impact", 0.04, false, false);
            projectile.enemy_impact_sprite = engine.graphics.get_animated_sprite("laser_o_enemy_impact", 0.04, false, false);
        }
        else {
            projectile.anim_sprite = engine.graphics.get_animated_sprite("laser_g", 0.04, true, false);
            projectile.wall_impact_sprite = engine.graphics.get_animated_sprite("laser_g_wall_impact", 0.04, false, false);
            projectile.enemy_impact_sprite = engine.graphics.get_animated_sprite("laser_g_enemy_impact", 0.04, false, false);
        }
        prev_gun_level = gun_level;
    }
}

std::pair<Vec<double>, Color> Player::get_sprite() const {
    return {physics.position, color};
}

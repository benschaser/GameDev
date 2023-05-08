#include "player.h"
#include "engine.h"
#include "combat.h"
#include <iostream>

Player::Player(Engine& engine, const Vec<double>& position, const Vec<int>& size)
    :size{size} {
        physics.position = position;
        physics.acceleration.y = gravity;
        combat.health = 5;
        combat.max_health = 5;
        combat.attack_damage = 3;
        standing = engine.graphics.get_animated_sprite("astronaut_standing", 0.1, false, false);
        jumping = engine.graphics.get_animated_sprite("astronaut_jumping", 0.1, false, false);
        running = engine.graphics.get_animated_sprite("astronaut_running", 0.05, false, false);
        falling = engine.graphics.get_animated_sprite("astronaut_falling", 0.1, false, false);
        grounding = engine.graphics.get_animated_sprite("astronaut_grounding", 0.04, false, false);


        sprite = standing.get_sprite();

        projectile.sprite = engine.graphics.get_sprite("arrow");
        projectile.combat.invincible = true;
        projectile.combat.attack_damage = 1;
        projectile.physics.acceleration.y = gravity;

        state = std::make_unique<Standing>();
        state->enter(*this, engine);
    }

std::unique_ptr<Command> Player::handle_input(const SDL_Event& event, Engine& engine) {
    auto new_state = state->handle_input(*this, event);
    if (new_state) {
        // std::cout << "exiting state\n";
        state->exit(*this, engine);
        // std::cout << "entering new state\n";
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
    
}

std::pair<Vec<double>, Color> Player::get_sprite() const {
    return {physics.position, color};
}

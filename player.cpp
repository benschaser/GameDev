#include "player.h"
#include "world.h"
#include <iostream>

Player::Player(const Vec<double>& position, const Vec<int>& size)
    :size{size} {
        physics.position = position;
        physics.acceleration.y = gravity;

        state = std::make_unique<Standing>();
        state->enter(*this);
    }

void Player::handle_input(const SDL_Event& event) {
    // state->handle_input(*this, event);
    auto new_state = state->handle_input(*this, event);
    if (new_state) {
        state->exit(*this);
        state = std::move(new_state);
        state->enter(*this);
    }
    // if (event.type == SDL_KEYDOWN) {
    //     SDL_Keycode key = event.key.keysym.sym;
    //     if (key == SDLK_RIGHT) {
    //         acceleration.x = walk_acceleration;
    //         // std::cout << "Right " << acceleration.x << '\n';
    //     }
    //     else if (key == SDLK_LEFT) {
    //         acceleration.x = -walk_acceleration;
    //     }
    //     if (key == SDLK_SPACE) {
    //         velocity.y = jump_velocity;
    //         acceleration.y = gravity;
    //     }
    // }
    // if (event.type == SDL_KEYUP) {
    //     SDL_Keycode key = event.key.keysym.sym;
    //     if (key == SDLK_RIGHT) {
    //         acceleration.x = 0.0;
    //     }
    //     else if (key == SDLK_LEFT) {
    //         acceleration.x = 0.0;
    //     }
    // }
}

void Player::update(World& world, double dt) {
    auto new_state = state->update(*this, world, dt);
    if (new_state) {
        state->exit(*this);
        state = std::move(new_state);
        state->enter(*this);
    }
}

std::pair<Vec<double>, Color> Player::get_sprite() const {
    return {physics.position, color};
}

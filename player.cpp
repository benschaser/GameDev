#include "player.h"
#include "world.h"
#include <iostream>

constexpr double walk_acceleration = 60.0;
constexpr double terminal_velocity = 160.0;
constexpr double jump_velocity = 16;
constexpr double gravity = -30;
constexpr double damping = 0.92;

Player::Player(const Vec<double>& position, const Vec<int>& size)
    :position{position}, size{size} {
        acceleration.y = gravity;
    }

void Player::handle_input(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_RIGHT) {
            acceleration.x = walk_acceleration;
            // std::cout << "Right " << acceleration.x << '\n';
        }
        else if (key == SDLK_LEFT) {
            acceleration.x = -walk_acceleration;
        }
        if (key == SDLK_SPACE) {
            velocity.y = jump_velocity;
            acceleration.y = gravity;
        }
    }
    if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_RIGHT) {
            acceleration.x = 0.0;
        }
        else if (key == SDLK_LEFT) {
            acceleration.x = 0.0;
        }
    }
}

void Player::update(World& world, double dt) {
    // update physics using semi-implicit euler
    velocity += acceleration * dt;
    velocity.x *= damping;
    // keep velocity under terminal velocity
    velocity.x = std::clamp(velocity.x, -terminal_velocity, terminal_velocity);
    velocity.y = std::clamp(velocity.y, -terminal_velocity, terminal_velocity);

    Vec<double> displacement = velocity * dt;

    // attempt to move in x first
    Vec<double> future{position.x + displacement.x, position.y};
    Vec<double> vx{velocity.x, 0};
    world.move_to(future, size, vx);

    // attempt to move in y
    Vec<double> vy{0, velocity.y};
    future.y += displacement.y;
    world.move_to(future, size, vy);

    // update player pos
    position = future;
    velocity.x = vx.x;
    velocity.y = vy.y;
}

std::pair<Vec<double>, Color> Player::get_sprite() const {
    return {position, {255, 0, 0, 255}};
}

#include "fsm.h"
#include "player.h"
#include "world.h"

//////////////////
// State
//////////////////
std::unique_ptr<State> State::update(Player& player, World& world, double dt) {
    Physics old = player.physics;
    player.physics.update(dt);

    // attempt to move in x first
    Vec<double> future{player.physics.position.x, old.position.y};
    Vec<double> vx{player.physics.velocity.x, 0};
    world.move_to(future, player.size, vx);

    // attempt to move in y
    Vec<double> vy{0, player.physics.velocity.y};
    future.y = player.physics.position.y;
    world.move_to(future, player.size, vy);

    // update player pos
    player.physics.position = future;
    player.physics.velocity = {vx.x, vy.y};

    // landing on platform
    

    return nullptr;
}

//////////////////
// Standing
//////////////////
std::unique_ptr<State> Standing::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        }
        else if (key == SDLK_RIGHT) {
            player.physics.acceleration.x = player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (key == SDLK_LEFT) {
            player.physics.acceleration.x = -player.walk_acceleration;
            return std::make_unique<Walking>();
        }
    }
    return nullptr;
}

std::unique_ptr<State> Standing::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    player.physics.velocity.x *= damping;

    // slide off platform -> return new state
    return nullptr;
}

void Standing::enter(Player& player) {
    player.physics.velocity.y = 0.0;
    player.color = {255, 0, 0, 255};
}

//////////////////
// Walking
//////////////////
std::unique_ptr<State> Walking::handle_input(Player&, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            return std::make_unique<Jumping>();
        }
    }
    return nullptr;
}

std::unique_ptr<State> Walking::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    return nullptr;
}

void Walking::enter(Player& player) {
    player.physics.velocity.y = 0.0;
    player.color = {200, 160, 160, 255};
}

//////////////////
// Jumping
//////////////////
std::unique_ptr<State> Jumping::handle_input(Player&, const SDL_Event& event) {
    // if (event.type == SDL_KEYDOWN) {
    //     SDL_Keycode key = event.key.keysym.sym;
    //     if (key == SDLK_DOWN) {
    //         return std::make_unique<Ground>();
    //     }
    // }

    // reduced movement in x axis
    // space -> add small boosted jump
    // ground pound :)
    return nullptr;
}

std::unique_ptr<State> Jumping::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    if (player.physics.velocity.y == 0) {
        // Vec<double> left_foot{player.physics.position.x, player.physics.position.y - 1e-2};
        return std::make_unique<Standing>();
    }

    return nullptr;
}

void Jumping::enter(Player& player) {
    player.physics.acceleration.x = 0;
    player.physics.velocity.y = player.jump_velocity;
    player.color = {0, 0, 255, 255};
}

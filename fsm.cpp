#include "fsm.h"
#include "player.h"
#include "world.h"
#include <iostream>


bool on_platform(const Player& player, const World& world) {
    constexpr double epsilon = 1e-2;
    Vec<double> left_foot{player.physics.position.x, player.physics.position.y - epsilon};
    Vec<double> right_foot{player.physics.position.x+player.size.x, player.physics.position.y - epsilon};
    return world.collides(left_foot) || world.collides(right_foot);
}

bool arrow_left = false;
bool arrow_right = false;

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
    
    // std::cout << "Left=" << arrow_left << "  Right=" << arrow_right << '\n';
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
        else if (key == SDLK_LEFT) {
            arrow_left = true;
            arrow_right = false;
            player.physics.acceleration.x = -player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (key == SDLK_RIGHT) {
            arrow_left = false;
            arrow_right = true;
            player.physics.acceleration.x = player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        
    }
    else if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            arrow_left = false;
            arrow_right = false;
            return std::make_unique<Standing>();
        }
    }
    
    return nullptr;
}

std::unique_ptr<State> Standing::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    player.physics.velocity.x *= damping;

    if (player.physics.velocity.y < 0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Standing::enter(Player& player) {
    player.next_command = std::make_unique<Stop>();
}

//////////////////
// Walking
//////////////////
std::unique_ptr<State> Walking::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_SPACE || key == SDLK_UP) {
            // player.physics.velocity.y = player.jump_velocity;
            return std::make_unique<Jumping>();
        }
    }
    else if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            arrow_left = false;
            arrow_right = false;
            return std::make_unique<Standing>();
        }
    }
    return nullptr;
}

std::unique_ptr<State> Walking::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);

    if (player.physics.velocity.y < 0.0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Walking::enter(Player& player) {
    player.physics.velocity.y = 0.0;
    // player.next_command = std::make_unique<Accelerate>(player.physics.acceleration.x);
    player.color = {200, 160, 160, 255};
}

//////////////////
// Jumping
//////////////////
std::unique_ptr<State> Jumping::handle_input(Player& player, const SDL_Event& event) {
     if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            arrow_left = false;
            arrow_right = false;
            return std::make_unique<Standing>();
        }
    }

    return nullptr;
}

std::unique_ptr<State> Jumping::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    return std::make_unique<InAir>();
}

void Jumping::enter(Player& player) {
    // player.next_command = std::make_unique<Jump>(player.jump_velocity);
    player.color = {0, 0, 255, 255};
    player.physics.acceleration.x = 0;
    player.physics.velocity.y = player.jump_velocity;
}

//////////////////
// InAir
//////////////////
std::unique_ptr<State> InAir::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_DOWN && arrow_left) {
            player.physics.velocity.x = -diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN && arrow_right) {
            player.physics.velocity.x = diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN) {
            return std::make_unique<GroundPounding>();
        }
        else if (key == SDLK_LEFT) {
            arrow_left = true;
            arrow_right = false;
            player.physics.acceleration.x = -in_air_acceleration;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = true;
            arrow_left = false;
            player.physics.acceleration.x = in_air_acceleration;
        }
    }
    else if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT) {
            arrow_left = false;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = false;
        }
    }
    return nullptr;
}

std::unique_ptr<State> InAir::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);

    if (on_platform(player, world) && player.physics.velocity.y == 0) {
        if (arrow_left) {
            return std::make_unique<Walking>();
        }
        else if (arrow_right) {
            return std::make_unique<Walking>();
        }
        else {
            return std::make_unique<Standing>();
        }
    }

    return nullptr;
}

void InAir::enter(Player& player) {
    // player.next_command = std::make_unique<GroundPound>();
    player.color = {0, 0, 255, 255};
}

//////////////////
// GroundPounding
//////////////////
std::unique_ptr<State> GroundPounding::handle_input(Player& player, const SDL_Event& event) {
    return nullptr;
}

std::unique_ptr<State> GroundPounding::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    if (on_platform(player, world) && player.physics.velocity.y == 0) {
        return std::make_unique<Standing>();
    }

    return nullptr;
}

void GroundPounding::enter(Player& player) {
    // player.next_command = std::make_unique<GroundPound>();
    player.color = {255, 0, 255, 255};
    player.physics.velocity.x = 0.0;
    player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// Diving
//////////////////
std::unique_ptr<State> Diving::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT) {
            arrow_left = false;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = false;
        }
    }
    return nullptr;
    return nullptr;
}

std::unique_ptr<State> Diving::update(Player& player, World& world, double dt) {
    State::update(player, world, dt);
    if (on_platform(player, world)) {
        if (arrow_left) {
            player.physics.acceleration.x = -player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (arrow_right) {
            player.physics.acceleration.x = player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        return std::make_unique<Standing>();
    }

    return nullptr;
}

void Diving::enter(Player& player) {
    // player.next_command = std::make_unique<Dive>(groundpound_velocity, player.physics.velocity.x);
    player.physics.velocity.y = groundpound_velocity;
    player.color = {0, 255, 255, 255};
}

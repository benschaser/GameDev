#include "fsm.h"
#include "player.h"
#include "engine.h"
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
std::unique_ptr<State> State::update(Player& player, Engine& engine, double dt) {
    Physics old = player.physics;
    player.physics.update(dt);

    // attempt to move in x first
    Vec<double> future{player.physics.position.x, old.position.y};
    Vec<double> vx{player.physics.velocity.x, 0};
    engine.world->move_to(future, player.size, vx);

    // attempt to move in y
    Vec<double> vy{0, player.physics.velocity.y};
    future.y = player.physics.position.y;
    engine.world->move_to(future, player.size, vy);

    // update player pos
    player.physics.position = future;
    player.physics.velocity = {vx.x, vy.y};

    // observe if current tile has command
    auto command = engine.world->touch_tiles(player);
    if (command) {
        command->execute(player, engine);
    }    
    
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
        else if (key == SDLK_f) {
            return std::make_unique<AttackAll>();
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

std::unique_ptr<State> Standing::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    player.physics.velocity.x *= damping;

    player.standing.update(dt);
    player.sprite = player.standing.get_sprite();

    if (player.physics.velocity.y < 0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Standing::enter(Player& player, Engine&) {
    // player.color = {255, 0, 0, 255};
    
    player.standing.reset();
    player.standing.flip(player.sprite.flip);
    player.next_command = std::make_unique<Stop>();
}

//////////////////
// Walking
//////////////////
std::unique_ptr<State> Walking::handle_input(Player&, const SDL_Event& event) {
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

std::unique_ptr<State> Walking::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    // engine.audio.play_sound("running");

    player.running.update(dt);
    player.sprite = player.running.get_sprite();

    if (player.physics.velocity.y < 0.0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Walking::enter(Player& player, Engine& engine) {
    // player.physics.velocity.y = 0.0;
    player.next_command = std::make_unique<Accelerate>(player.physics.acceleration.x);
    player.running.reset();
    player.running.flip(player.physics.acceleration.x < 0);
    engine.audio.play_sound("running", false, true);
    player.color = {200, 160, 160, 255};
}

void Walking::exit(Player&, Engine& engine) {
    engine.audio.stop_sound();
}

//////////////////
// Jumping
//////////////////
std::unique_ptr<State> Jumping::handle_input(Player&, const SDL_Event& event) {
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

std::unique_ptr<State> Jumping::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    return std::make_unique<InAir>();
}

void Jumping::enter(Player& player, Engine& engine) {
    player.next_command = std::make_unique<Jump>(player.jump_velocity);
    // player.color = {0, 0, 255, 255};
    engine.audio.play_sound("jumping");
    // player.physics.acceleration.x = 0;
    // player.physics.velocity.y = player.jump_velocity;
}

//////////////////
// InAir
//////////////////
std::unique_ptr<State> InAir::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_DOWN && arrow_left) {
            player.falling.flip(true);
            player.jumping.flip(true);
            player.physics.velocity.x = -diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN && arrow_right) {
            player.falling.flip(false);
            player.jumping.flip(false);
            player.physics.velocity.x = diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN) {
            return std::make_unique<GroundPounding>();
        }
        else if (key == SDLK_LEFT) {
            arrow_left = true;
            arrow_right = false;
            player.falling.flip(true);
            player.jumping.flip(true);
            player.physics.acceleration.x = -in_air_acceleration;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = true;
            arrow_left = false;
            player.falling.flip(false);
            player.jumping.flip(false);
            player.physics.acceleration.x = in_air_acceleration;
        }
    }
    else if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT) {
            arrow_left = false;
            player.physics.acceleration.x = 0;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = false;
            player.physics.acceleration.x = 0;
        }
    }

    return nullptr;
}

std::unique_ptr<State> InAir::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);

    if (on_platform(player, *engine.world) && player.physics.velocity.y == 0) {
        if (arrow_left) {
            player.physics.acceleration.x = -player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (arrow_right) {
            player.physics.acceleration.x = player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else {
            engine.audio.play_sound("landing");
            return std::make_unique<Standing>();
        }
    }
    if (player.physics.velocity.y < 0) {
        player.sprite = player.falling.get_sprite();
    }
    
    else {
        player.sprite = player.jumping.get_sprite();
    }

    return nullptr;
}

void InAir::enter(Player& player, Engine&) {
    player.next_command = std::make_unique<Accelerate>(player.physics.acceleration.x);
    player.jumping.flip(player.physics.velocity.x < 0);
    player.falling.flip(player.physics.velocity.x < 0);
}

//////////////////
// GroundPounding
//////////////////
std::unique_ptr<State> GroundPounding::handle_input(Player&, const SDL_Event&) {
    return nullptr;
}

std::unique_ptr<State> GroundPounding::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    player.grounding.update(dt);

    player.sprite = player.grounding.get_sprite();
    if (on_platform(player, *engine.world) && player.physics.velocity.y == 0) {
        return std::make_unique<Standing>();
    }

    return nullptr;
}

void GroundPounding::enter(Player& player, Engine&) {
    player.next_command = std::make_unique<GroundPound>();
    // player.physics.velocity.x = 0.0;
    // player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// Diving
//////////////////
std::unique_ptr<State> Diving::handle_input(Player&, const SDL_Event& event) {
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
}

std::unique_ptr<State> Diving::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    if (on_platform(player, *engine.world)) {
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

void Diving::enter(Player& player, Engine&) {
    player.next_command = std::make_unique<Dive>(player.physics.velocity.x);
    // player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// AttackAll
//////////////////
std::unique_ptr<State> AttackAll::handle_input(Player& player, const SDL_Event& event) {
    // std::cout << "handling input\n";
    if (event.type == SDL_KEYUP) {
        // std::cout << "keyup event\n";
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_f) {
            // std::cout << "leaving attack state\n";
            return std::make_unique<Standing>();
        }
    }
    return nullptr;
}

void AttackAll::enter(Player& player, Engine& engine) {
    for (auto enemy : engine.world->enemies) {
        player.combat.attack(*enemy);
    }
}

//////////////////
// Hurting
//////////////////
std::unique_ptr<State> Hurting::handle_input(Player& player, const SDL_Event& event) {
    // std::cout << "handling input\n";
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT) {
            player.next_command = std::make_unique<Accelerate>(-player.walk_acceleration / 4);
            player.sprite.flip = true;
        }
        else if (key == SDLK_RIGHT) {
            player.next_command = std::make_unique<Accelerate>(player.walk_acceleration / 4);
            player.sprite.flip = false;
        }
    }
    if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT || key == SDLK_RIGHT) {
            player.next_command = std::make_unique<Stop>();
        }
    }
    return nullptr;
}

std::unique_ptr<State> Hurting::update(Player& player, Engine& engine, double dt) {
    elapsed_time += dt;
    if (elapsed_time >= cooldown) {
        return std::make_unique<Standing>();
    }

    State::update(player, engine, dt);    

    return nullptr;
}

void Hurting::enter(Player& player, Engine& engine) {
    elapsed_time = 0;
    player.combat.invincible = true;
}

void Hurting::exit(Player& player, Engine& engine) {
    player.combat.invincible = false;
}

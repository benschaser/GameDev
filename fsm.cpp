#include "fsm.h"
#include "player.h"
#include "engine.h"
#include <iostream>
#include <randomness.h>


bool on_platform(const Player& player, const World& world) {
    constexpr double epsilon = 1e-2;
    Vec<double> left_foot{player.physics.position.x, player.physics.position.y - epsilon};
    Vec<double> right_foot{player.physics.position.x+player.size.x, player.physics.position.y - epsilon};
    return world.collides(left_foot) || world.collides(right_foot);
}

bool arrow_left = false;
bool arrow_right = false;
bool flip = false;

//////////////////
// State
//////////////////
std::unique_ptr<State> State::update(Player& player, Engine& engine, double dt) {
    Physics old = player.physics;
    player.physics.update(dt);
    if (player.carrying_elapsed >= player.holster_cooldown) {
        player.carrying = false;
    }
    player.elapsed += dt;
    player.carrying_elapsed += dt;
    
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
            flip = true;
            player.physics.acceleration.x = -player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (key == SDLK_RIGHT) {
            arrow_left = false;
            arrow_right = true;
            flip = false;
            player.physics.acceleration.x = player.walk_acceleration;
            return std::make_unique<Walking>();
        }
        else if (key == SDLK_f && player.elapsed >= player.cooldown) {
            player.elapsed = 0;
            player.carrying_elapsed = 0;
            player.carrying = true;
            player.next_command = std::make_unique<Fire>(player);
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
    player.standing_carrying_g.update(dt);
    player.standing_carrying_o.update(dt);
    player.standing_carrying_p.update(dt);

    if (player.carrying) {
        if (player.gun_level == 2) {
            player.sprite = player.standing_carrying_p.get_sprite();
        }
        else if (player.gun_level == 1) {
            player.sprite = player.standing_carrying_o.get_sprite();
        }
        else {
            player.sprite = player.standing_carrying_g.get_sprite();
        }
        
    }
    else {
        player.sprite = player.standing.get_sprite();
    }

    if (player.physics.velocity.y < 0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Standing::enter(Player& player, Engine&) {    
    player.standing.reset();
    player.standing.flip(flip);
    player.standing_carrying_g.reset();
    player.standing_carrying_g.flip(flip);
    player.standing_carrying_o.reset();
    player.standing_carrying_o.flip(flip);
    player.standing_carrying_p.reset();
    player.standing_carrying_p.flip(flip);
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
        else if (key == SDLK_f && player.elapsed >= player.cooldown) {
            player.elapsed = 0;
            player.carrying_elapsed = 0;
            player.carrying = true;
            player.next_command = std::make_unique<Fire>(player);
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
    player.running.update(dt);
    player.running_carrying_g.update(dt);
    player.running_carrying_o.update(dt);
    player.running_carrying_p.update(dt);
    
    if (player.carrying) {
        if (player.gun_level == 2) {
            player.sprite = player.running_carrying_p.get_sprite();
        }
        else if (player.gun_level == 1) {
            player.sprite = player.running_carrying_o.get_sprite();
        }
        else {
            player.sprite = player.running_carrying_g.get_sprite();
        }
    }
    else {
        player.sprite = player.running.get_sprite();
    }

    if (player.physics.velocity.y < 0.0) {
        return std::make_unique<InAir>();
    }

    return nullptr;
}

void Walking::enter(Player& player, Engine& engine) {
    player.next_command = std::make_unique<Accelerate>(player.physics.acceleration.x);
    player.running.reset();
    player.running.flip(flip);
    player.running_carrying_g.reset();
    player.running_carrying_g.flip(flip);
    player.running_carrying_o.reset();
    player.running_carrying_o.flip(flip);
    player.running_carrying_p.reset();
    player.running_carrying_p.flip(flip);
    engine.audio.play_sound("running", false, true);
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
    if (flip) {
        player.sprite.flip = true;
    }
    engine.audio.play_sound("jumping");
}

//////////////////
// InAir
//////////////////
std::unique_ptr<State> InAir::handle_input(Player& player, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_DOWN && arrow_left) {
            flip = true;
            player.falling.flip(true);
            player.jumping.flip(true);
            player.physics.velocity.x = -diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN && arrow_right) {
            flip = false;
            player.falling.flip(false);
            player.jumping.flip(false);
            player.physics.velocity.x = diving_velocity;
            return std::make_unique<Diving>();
        }
        else if (key == SDLK_DOWN) {
            return std::make_unique<GroundPounding>();
        }
        else if (key == SDLK_LEFT) {
            flip = true;
            arrow_left = true;
            arrow_right = false;
            player.falling.flip(true);
            player.jumping.flip(true);
            player.physics.acceleration.x = -in_air_acceleration;
        }
        else if (key == SDLK_RIGHT) {
            flip = false;
            arrow_right = true;
            arrow_left = false;
            player.falling.flip(false);
            player.jumping.flip(false);
            player.physics.acceleration.x = in_air_acceleration;
        }
        else if (key == SDLK_f && player.elapsed >= player.cooldown) {
            player.elapsed = 0;
            player.carrying_elapsed = 0;
            player.carrying = true;
            player.next_command = std::make_unique<Fire>(player);
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
    player.jumping_carrying_g.update(dt);
    player.jumping_carrying_o.update(dt);
    player.jumping_carrying_p.update(dt);

    if (player.carrying) {
        if (player.gun_level == 2) {
            player.sprite = player.jumping_carrying_p.get_sprite();
        }
        else if (player.gun_level == 1) {
            player.sprite = player.jumping_carrying_o.get_sprite();
        }
        else {
            player.sprite = player.jumping_carrying_g.get_sprite();
        }
    }
    else if (player.physics.velocity.y < 0) {
        player.sprite = player.falling.get_sprite();
    }
    else {
        player.sprite = player.jumping.get_sprite();
    }

    player.sprite.flip = flip;
    return nullptr;
}

void InAir::enter(Player& player, Engine&) {
    player.next_command = std::make_unique<Accelerate>(player.physics.acceleration.x);
    if (flip) {
        player.jumping.flip(true);
        player.falling.flip(true);
    }
    
}

//////////////////
// GroundPounding
//////////////////
std::unique_ptr<State> GroundPounding::handle_input(Player&, const SDL_Event&) {
    return nullptr;
}

std::unique_ptr<State> GroundPounding::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt);
    player.combat.attack_damage = 10;
    player.grounding.update(dt);

    player.sprite = player.grounding.get_sprite();
    if (on_platform(player, *engine.world) && player.physics.velocity.y == 0) {
        return std::make_unique<Standing>();
    }

    return nullptr;
}

void GroundPounding::enter(Player& player, Engine& engine) {
    player.next_command = std::make_unique<GroundPound>();
    player.grounding.flip(flip);
    engine.audio.play_sound("grounding");
    player.grounded = true;
}
void GroundPounding::exit(Player& player, Engine& engine) {
    engine.audio.play_sound("landing");
    player.grounded = false;

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
}

//////////////////
// AttackAll
//////////////////
std::unique_ptr<State> AttackAll::handle_input(Player&, const SDL_Event& event) {
    if (event.type == SDL_KEYUP) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_f) {
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
std::unique_ptr<State> Hurting::handle_input(Player&, const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_LEFT) {
            arrow_left = true;
        }
        else if (key == SDLK_RIGHT) {
            arrow_right = true;
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

std::unique_ptr<State> Hurting::update(Player& player, Engine& engine, double dt) {
    State::update(player, engine, dt); 
    if (!player.combat.is_alive) {
        return std::make_unique<Dying>();
    }   
    if (on_platform(player, *engine.world)) {
        player.physics.velocity.x = 0;
        return std::make_unique<Standing>();
    }
    return nullptr;
}

void Hurting::enter(Player& player, Engine& engine) {
    engine.audio.play_sound("hurt");
    player.physics.velocity.x = -4;
    player.physics.velocity.y = 4;
    if (player.sprite.flip) {
        player.physics.velocity.x = 4;
    }
    player.physics.acceleration = {0, gravity};
    player.combat.invincible = true;
}

void Hurting::exit(Player& player, Engine&) {
    player.combat.invincible = false;
}

//////////////////
// Dying
//////////////////
std::unique_ptr<State> Dying::handle_input(Player&, const SDL_Event&) {
    return nullptr;
}

std::unique_ptr<State> Dying::update(Player& player, Engine&, double dt) {
    elapsed_time += dt;
    if (elapsed_time >= cooldown) {
        player.next_command = std::make_unique<EndGame>();
    }
    else {
        player.dying.update(dt);
        player.sprite = player.dying.get_sprite();
    }
    return nullptr;
}

void Dying::enter(Player& player, Engine& engine) {
    engine.audio.play_sound("player_death");
    elapsed_time = 0;
    player.physics.velocity.x = 0;
    player.physics.velocity.y = 0;
    player.dying.flip(player.sprite.flip);
    player.sprite = player.dying.get_sprite();
    
}

void Dying::exit(Player& player, Engine&) {
    player.next_command = std::make_unique<EndGame>();
}

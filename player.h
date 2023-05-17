#pragma once

#include "graphics.h"
#include "vec.h"
#include "physics.h"
#include "fsm.h"
#include "command.h"
#include "entity.h"
#include "projectile.h"
#include <SDL2/SDL.h>
#include <memory>

// forward declaration
class Engine;

class Player : public Entity {
public:
    Player(Engine& engine, const Vec<double>& position, const Vec<int>& size);

    std::unique_ptr<Command> handle_input(const SDL_Event& event, Engine& engine);
    void update(Engine& engine, double dt);
    std::pair<Vec<double>, Color> get_sprite() const;
    const double walk_acceleration = 80.0;
    const double jump_velocity = 16;
    Vec<int> size;
    Color color{255, 0, 0, 255};
    AnimatedSprite standing;
    AnimatedSprite standing_carrying_g;
    AnimatedSprite standing_carrying_o;
    AnimatedSprite standing_carrying_p;
    AnimatedSprite jumping;
    AnimatedSprite jumping_carrying_g;
    AnimatedSprite jumping_carrying_o;
    AnimatedSprite jumping_carrying_p;
    AnimatedSprite running;
    AnimatedSprite running_carrying_g;
    AnimatedSprite running_carrying_o;
    AnimatedSprite running_carrying_p;
    AnimatedSprite dying;
    AnimatedSprite falling;
    AnimatedSprite grounding;
    std::unique_ptr<State> state;
    std::unique_ptr<Command> next_command;

    Projectile projectile;
    double cooldown{0.16}, elapsed{0}, holster_cooldown{2.0}, carrying_elapsed{0};
    bool grounded{false};
    int prev_gun_level{0};
};

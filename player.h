#pragma once

#include "graphics.h"
#include "vec.h"
#include "physics.h"
#include "fsm.h"
#include "command.h"
#include <SDL2/SDL.h>
#include <memory>

// forward declaration
class Engine;

class Player {
public:
    Player(Engine& engine, const Vec<double>& position, const Vec<int>& size);

    std::unique_ptr<Command> handle_input(const SDL_Event& event, Engine& engine);
    void update(Engine& engine, double dt);
    std::pair<Vec<double>, Color> get_sprite() const;
    Physics physics;
    const double walk_acceleration = 80.0;
    const double jump_velocity = 16;
    Vec<int> size;
    Color color{255, 0, 0, 255};
    Sprite sprite;
    AnimatedSprite standing;
    AnimatedSprite jumping;
    AnimatedSprite running;
    AnimatedSprite falling;
    AnimatedSprite grounding;
    std::unique_ptr<State> state;
    std::unique_ptr<Command> next_command;
};

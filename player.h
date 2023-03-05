#pragma once

#include "graphics.h"
#include "vec.h"
#include "physics.h"
#include "fsm.h"
#include <SDL2/SDL.h>
#include <memory>

// forward declaration
class World;

class Player {
public:
    Player(const Vec<double>& position, const Vec<int>& size);
    void handle_input(const SDL_Event& event);
    void update(World& world, double dt);
    std::pair<Vec<double>, Color> get_sprite() const;
    Physics physics;
    const double walk_acceleration = 60.0;
    const double jump_velocity = 16;
    Vec<int> size;
    std::unique_ptr<State> state;
    Color color{255, 0, 0, 255};
};

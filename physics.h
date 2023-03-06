#pragma once
#include "vec.h"

constexpr double gravity = -30;
constexpr double damping = 0.92;
constexpr double terminal_velocity = 160.0;

class Physics {
public:
    void update(double dt);
    Vec<double> position, velocity, acceleration{0, gravity};
};
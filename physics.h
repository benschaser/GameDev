#pragma once
#include "vec.h"

constexpr double gravity = -20;
constexpr double damping = 0.8;
constexpr double terminal_velocity = 16.0;
constexpr double in_air_acceleration = 10.0;
constexpr double in_air_terminal_velocity = 5.0;
constexpr double groundpound_velocity = -15;
constexpr double diving_velocity = 10.0;

class Physics {
public:
    void update(double dt);
    Vec<double> position, velocity, acceleration{0, gravity};
};
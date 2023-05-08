#pragma once

#include "entity.h"

class Engine;

class Projectile : public Entity {
public:
    void update(Engine& engine, double dt);
    Vec<int> size{1,1};
    double lifetime{1}, elapsed{0};
};
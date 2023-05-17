#pragma once

#include "entity.h"
#include "animatedsprite.h"

class Engine;

class Projectile : public Entity {
public:
    void update(Engine& engine, double dt);
    Vec<int> size{1,1};
    double lifetime{0.12}, elapsed{0};
    AnimatedSprite anim_sprite;
    AnimatedSprite wall_impact_sprite;
    AnimatedSprite enemy_impact_sprite;
    bool hit_enemy = false;
};
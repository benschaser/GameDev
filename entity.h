#pragma once

#include "physics.h"
#include "sprite.h"
#include "combat.h"

class Entity {
public:
    Physics physics;
    Sprite sprite;
    Combat combat;
    int gun_level{0};
    bool carrying{false};
};
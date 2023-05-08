#pragma once

#include "physics.h"
#include "sprite.h"
#include "combat.h"

class Entity {
public:
    Physics physics;
    Sprite sprite;
    Combat combat;
};
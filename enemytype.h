#pragma once

#include "animatedsprite.h"
#include "command.h"
#include "graphics.h"

#include <functional>

class Enemy;
class Engine;

class EnemyType {
public:
    AnimatedSprite animation;
    Vec<double> acceleration;
    int health, damage;
    double cooldown, elapsed_time;
    std::function<std::unique_ptr<Command>(Engine& engine, Enemy& enemy)> behavior;
    double x_velocity_max;
    
    
};

EnemyType create_enemytype(Graphics& graphics, std::string type_name);

std::unique_ptr<Command> default_behavior(Engine&, Enemy& enemy);
std::unique_ptr<Command> standing_behavior(Engine&, Enemy& enemy);
std::unique_ptr<Command> hurting_behavior(Engine&, Enemy& enemy);


EnemyType create_troll(Graphics& graphics);
EnemyType create_monster(Graphics& graphics);
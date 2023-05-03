#include "enemytype.h"
#include "enemy.h"

EnemyType create_enemytype(Graphics& graphics, std::string type_name) {
    // EnemyType type;
    // physics.acceleration = type.acceleration;
    // type.animation = graphics.get_animated_sprite(type_name + "_running", 0.1, true, false);
    // type.behavior = default_behavior;
    // return type;
    if (type_name == "troll") {
        return create_troll(graphics);
    }
    else if (type_name == "monster") {
        return create_monster(graphics);
    }
    else {
        throw std::runtime_error("Unknown typename: " + type_name);
    }
    
}

std::unique_ptr<Command> default_behavior(Engine&, Enemy& enemy) {
    if (abs(enemy.last_edge_position.x - enemy.physics.position.x) > 5) {
        enemy.last_edge_position.x = enemy.physics.position.x;
        enemy.physics.acceleration.x = -enemy.physics.acceleration.x;
    }

    return std::make_unique<Accelerate>(enemy.physics.acceleration.x);
}
std::unique_ptr<Command> standing_behavior(Engine&, Enemy&) {
    return std::make_unique<Stop>();
}

EnemyType create_troll(Graphics& graphics) {
    Vec<double> acceleration {-30, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("troll_running", 0.15, true, false);
    return EnemyType{sprite, acceleration, 8, 2, default_behavior, 2};
}

EnemyType create_monster(Graphics& graphics) {
    Vec<double> acceleration {0, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("monster_standing", 0.15, true, false);
    return EnemyType{sprite, acceleration, 6, 3, default_behavior, 6};
}
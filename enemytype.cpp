#include "enemytype.h"
#include "enemy.h"

EnemyType create_enemytype(Graphics& graphics, std::string type_name) {
    if (type_name == "sentry") {
        return create_sentry(graphics);
    }
    else if (type_name == "ranger") {
        return create_ranger(graphics);
    }
    else if (type_name == "warden") {
        return create_warden(graphics);
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


std::unique_ptr<Command> hurting_behavior(Engine& engine, Enemy& enemy) {
    return default_behavior(engine, enemy);
}

EnemyType create_sentry(Graphics& graphics) {
    Vec<double> acceleration {0, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("sentry_standing", 0.1, true, false);
    AnimatedSprite death = graphics.get_animated_sprite("sentry_death", 0.1, false, false);
    sprite.flip(true);
    death.flip(true);
    death.loop = false;
    return EnemyType{sprite, death, acceleration, 24, 4, 0.01, 0, default_behavior, 6};
}

EnemyType create_ranger(Graphics& graphics) {
    Vec<double> acceleration {-16, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("ranger_walking", 0.1, true, false);
    AnimatedSprite death = graphics.get_animated_sprite("ranger_death", 0.1, false, false);
    death.loop = false;
    return EnemyType{sprite, death, acceleration, 8, 2, 0.01, 0, default_behavior, 5};
}

EnemyType create_warden(Graphics& graphics) {
    Vec<double> acceleration {-8, 0};
    AnimatedSprite sprite = graphics.get_animated_sprite("warden_walking", 0.1, true, false);
    AnimatedSprite death = graphics.get_animated_sprite("warden_death", 0.1, false, false);
    death.loop = false;
    return EnemyType{sprite, death, acceleration, 30, 6, 0.01, 0, default_behavior, 3};
}
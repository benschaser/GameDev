#pragma once

class Entity;

class Combat {
public:
    int health{0}, max_health{0}, attack_damage{0};
    bool is_alive = true;
    bool invincible = false;
    void attack(Entity& entity);
    void take_damage(int damage);
};
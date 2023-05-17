#include "combat.h"
#include "entity.h"

void Combat::attack(Entity& entity) {
    entity.combat.take_damage(attack_damage);
}

void Combat::take_damage(int damage) {
    if (invincible) {
        return;
    }
    invincible = true;
    health -= damage;
    if (health <= 0) {
        health = 0;
        is_alive = false;
    }
}
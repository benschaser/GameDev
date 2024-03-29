#pragma once

#include "tilemap.h"
#include "vec.h"
#include "level.h"
#include "command.h"
#include "enemy.h"
#include "quadtree.h"

class Player;

class World {
public:
    World(const Level& level);
    void move_to(Vec<double>& position, const Vec<int>& size, Vec<double>& velocity);
    bool collides(const Vec<double>& position) const;

    std::shared_ptr<Command> touch_tiles(const Player& player);
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::vector<Projectile> projectiles;

    Tilemap tilemap;
    std::vector<std::pair<Sprite, int>> backgrounds;
    QuadTree quadtree;
    void remove_inactive();
    void build_quadtree();
};

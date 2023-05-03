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
    // void add_platform(int x, int y, int width, int height);
    void move_to(Vec<double>& position, const Vec<int>& size, Vec<double>& velocity);
    bool collides(const Vec<double>& position) const;

    std::shared_ptr<Command> touch_tiles(const Player& player);
    std::vector<std::shared_ptr<Enemy>> enemies;

    Tilemap tilemap;
    std::vector<std::pair<Sprite, int>> backgrounds;
    QuadTree quadtree;

    void build_quadtree();
};

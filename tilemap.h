#pragma once
#include <vector>
#include "sprite.h"
#include "command.h"

class Tile {
public:
    AnimatedSprite sprite;
    bool blocking{false};
    std::shared_ptr<Command> command{nullptr};
};

class Tilemap {
public:
    Tilemap(int width, int height);
    Tile& operator()(int x, int y);
    const Tile& operator()(int x, int y) const;
    void update(double dt);
   


    const int width;
    const int height;
private:
    std::vector<Tile> tiles;
    void check_bounds(int x, int y) const; // error handling
};
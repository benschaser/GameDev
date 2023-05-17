#pragma once

#include "vec.h"
#include "entity.h"

// forward declarations
class Color;
class Graphics;
class Tilemap;
class Sprite;
class Player;
class Enemy;

class Camera {
public:
    Camera(Graphics& graphics, int tilesize);

    void move_to(const Vec<double>& new_location);
    void update(double dt);
    Vec<int> world_to_screen(const Vec<double>& world_position) const;

    void render(const Vec<double>& position, const Color& color, bool filled = true) const;
    void render(const Tilemap& tilemap, bool grid_on = false) const;
    void render(const Vec<double>& position, const Sprite& sprite) const;
    void render_screen(const Vec<int>& position, const Sprite& sprite) const;
    void render(const Entity& player) const;
    void render(const std::vector<std::pair<Sprite, int>>& backgrounds) const;
    void update_tiles(Tilemap& tilemap, double dt);
    void render_life(int current, int max);
    void render_enemy_health(const Enemy& enemy);

private:
    Graphics& graphics;
    int tilesize;
    Vec<double> location; // camera pos in world coordinates
    Vec<double> velocity = 0;
    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
};
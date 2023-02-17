#pragma once

#include "vec.h"

// forward declarations
class Color;
class Graphics;
class Tilemap;

class Camera {
public:
    Camera(Graphics& graphics, int tilesize);

    void move_to(const Vec<double>& new_location);
    Vec<int> world_to_screen(const Vec<double>& world_position) const;

    void render(const Vec<double>& position, const Color& color, bool fill = true) const;
    void render(const Tilemap& tilemao, bool grid_on = false) const;

private:
    Graphics& graphics;
    int tilesize;
    Vec<double> location; // camera pos in world coordinates

    void calculate_visible_tiles();
    Vec<int> visible_min, visible_max;
}
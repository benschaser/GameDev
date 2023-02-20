#include "camera.h"
#include "graphics.h"
#include "tilemap.h"

Camera::Camera(Graphics& graphics, int tilesize)
    :graphics{graphics}, tilesize{tilesize} {
        calculate_visible_tiles();
    }

void Camera::move_to(const Vec<double>& new_location) {
    location = new_location;
    calculate_visible_tiles();
}

Vec<int> Camera::world_to_screen(const Vec<double>& world_position) const {
    Vec<double> scaled = (world.position - location) * static_cast<double>(tilesize);
    Vec<int> pixel{static_cast<int>(scaled.x), static_cast<int>(scaled.y)};

    // shift to center of the screen
    pixel += Vec<int>(graphics.width/2, graphics.height/2);

    // flip y axis
    pixel.y = graphics.height - pixel.y;
    return pixel;
}

void render(const Vec<double>& position, const Color& color, bool filled = true) const {
    Vec<int> pixel = world_to_screen(position);
    // center tiles
    pixel -= Vec{tilesize / 2, tilesize / 2};
    SDL_Rect rect{pixel.x, pixel.y, tilesize, tilesize};
    graphics.draw(rect, color, filled);
}
void render(const Tilemap& tilemap, bool grid_on = false) const {
    // avoid out of bounds indices
    int xmin = std::max(0, visible_min.x);
    int ymin = std::max(0, visible_min.y);

    int xmax = std::min(visible_max.x, tilemap.width - 1);
    int ymax = std::min(visible_max.y, tilemap.height - 1);

    // draw tiles
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            const Tile& tile = tilemap(x, y);
            Vec<double> position{static_cast<double>(x), static_cast<double>(y)};
            if (tile == Tile::Platform) {
                render(position, {0, 255, 200, 255});
            }
            else {
                render(position, {0, 10, 36, 255});
            }

            if (grid_on) {
                render(position, {0, 0, 0, 255}, false);
            }
        }
    }
}

void Camera::calculate_visible_tiles() {
    // number of tiles visible (plus 1 for edges)
    Vec<int> num_tiles = Vec{graphics.width, graphics.height} / (2 * tilesize) + Vec{1, 1};
    Vec<int> center{static_cast<int>(location.x), static_cast<int>(location.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
}
#include "camera.h"

#include "graphics.h"
#include "tilemap.h"
#include "vec.h"
#include "player.h"
#include <iostream>

constexpr double v_factor = 2.5;
constexpr int lower_bound = 5; // I know this is dependent on window size but I'm not sure how to accomplish this cutoff without the world as an update parameter

Camera::Camera(Graphics& graphics, int tilesize)
    :graphics{graphics}, tilesize{tilesize} {
    calculate_visible_tiles();
}

void Camera::move_to(const Vec<double>& new_location) {
    velocity = (new_location - location) * v_factor;
}

void Camera::update(double dt) {
    location += velocity * dt;
    if (location.y < (graphics.height / 64) / 2 + 3) {
        // location.y = lower_bound;
        location.y = (graphics.height / 64) / 2 + 3;
    }
    else if (location.y > graphics.level_height - 7) {
        location.y = graphics.level_height - 7;
    }

    if (location.x < 16) {
        location.x = 16;
    }
    else if (location.x > graphics.level_width - 16) {
        location.x = graphics.level_width - 16;
    }
    calculate_visible_tiles();
}

Vec<int> Camera::world_to_screen(const Vec<double>& world_position) const {
    Vec<double> scaled =
        (world_position - location) * static_cast<double>(tilesize);
    Vec<int> pixel{static_cast<int>(scaled.x), static_cast<int>(scaled.y)};

    // shift to the center of the screen
    pixel += Vec<int>{graphics.width / 2, graphics.height / 2};

    // flip the y axis
    pixel.y = graphics.height - pixel.y;
    return pixel;
}

void Camera::render(const Vec<double>& position, const Color& color,
                    bool filled) const {
    Vec<int> pixel = world_to_screen(position);
    pixel -= Vec{tilesize / 2, tilesize / 2};  // centered on tile
    SDL_Rect rect{pixel.x, pixel.y, tilesize, tilesize};
    graphics.draw(rect, color, filled);
}

void Camera::render(const Tilemap& tilemap, bool grid_on) const {
    // screen to world conversion
    // calculate min and max world coordinates and only draw those
    int xmin = std::max(0, visible_min.x);
    int ymin = std::max(0, visible_min.y);
    int xmax = std::min(visible_max.x, tilemap.width - 1);
    int ymax = std::min(visible_max.y, tilemap.height - 1);

    // draw tiles
    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            const Tile& tile = tilemap(x, y);
            Vec<double> position{static_cast<double>(x), static_cast<double>(y)};
            render(position, tile.sprite);
            if (grid_on) {
                render(position, Color{0, 0, 0, 255}, false);
            }
        }
    }
}

void Camera::render(const Vec<double>& position, const Sprite& sprite) const {
    Vec<int> pixel = world_to_screen(position);
    pixel.y += tilesize / 2; // shift sprite bottom center down to bottom of tile
    graphics.draw_sprite(pixel, sprite);
}

void Camera::render(const Entity& player) const {
    // render(player.physics.position, player.color);
    render(player.physics.position, player.sprite);
}

void Camera::render(const std::vector<std::pair<Sprite, int>>& backgrounds) const {
    for (auto [sprite, distance] : backgrounds) {
        int shift = static_cast<int>(location.x / distance);
        graphics.draw_sprite({-shift, 0}, sprite);
    }
}

void Camera::calculate_visible_tiles() {
    // number of tiles visible (plus one for the edges)
    Vec<int> num_tiles = Vec{graphics.width, graphics.height};
    num_tiles /= (2 * tilesize);
    num_tiles += Vec{1, 1};
    // (Vec{graphics.width, graphics.height} / (2 * tilesize)) + Vec{1, 1};

    Vec<int> center{static_cast<int>(location.x), static_cast<int>(location.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
}

void Camera::render_life(int current, int max) {
    Sprite heart = graphics.get_sprite("heart");
    Sprite empty_heart = graphics.get_sprite("empty_heart");

    for (int i = 0; i < current; ++i) {
        Vec<int> position{35, 65};
        position.x += i * 64 + 10;
        graphics.draw_sprite(position, heart);
    }
    for (int i = current; i < max; ++i) {
        Vec<int> position{35, 65};
        position.x += i * 64 + 10;
        graphics.draw_sprite(position, empty_heart);
    }
}
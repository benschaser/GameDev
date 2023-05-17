#include "camera.h"

#include "graphics.h"
#include "tilemap.h"
#include "vec.h"
#include "player.h"
#include "enemy.h"
#include <iostream>

constexpr double v_factor = 2.5;

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
            // tilemap(x, y).sprite.update(dt);
            const Tile& tile = tilemap(x, y);
            Vec<double> position{static_cast<double>(x), static_cast<double>(y)};
            render(position, tile.sprite.get_sprite());
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

void Camera::render_screen(const Vec<int>& position, const Sprite& sprite) const {
    graphics.draw_sprite(position, sprite);
}

void Camera::render(const Entity& player) const {
    render(player.physics.position, player.sprite);
}

void Camera::render(const std::vector<std::pair<Sprite, int>>& backgrounds) const {
    for (auto [sprite, distance] : backgrounds) {
        int shift = static_cast<int>(location.x / (distance*0.5));
        graphics.draw_sprite({-shift, 0}, sprite);
    }
}

void Camera::update_tiles(Tilemap& tilemap, double dt) {
    tilemap.update(dt);
}

void Camera::calculate_visible_tiles() {
    // number of tiles visible (plus one for the edges)
    Vec<int> num_tiles = Vec{graphics.width, graphics.height};
    num_tiles /= (2 * tilesize);
    num_tiles += Vec{1, 1};

    Vec<int> center{static_cast<int>(location.x), static_cast<int>(location.y)};
    visible_max = center + num_tiles;
    visible_min = center - num_tiles;
}

void Camera::render_life(int current, int max) {
    SDL_Rect outline{32, 32, 480, 32};
    double health = (static_cast<double>(current) / static_cast<double>(max)) * 480;
    SDL_Rect fill{32, 32, static_cast<int>(health), 32};
    Color color{255, 50, 50, 255};
    graphics.draw(outline, color, false);
    graphics.draw(fill, color, true);

}
void Camera::render_enemy_health(const Enemy& enemy) {
    Vec<double> world_pos{enemy.physics.position.x - 0.5, enemy.physics.position.y + 2.3};
    Vec<int> screen_pos = world_to_screen(world_pos);
    SDL_Rect outline{screen_pos.x, screen_pos.y, 64, 8};
    double health = (static_cast<double>(enemy.combat.health) / static_cast<double>(enemy.combat.max_health)) * 64;
    SDL_Rect fill{screen_pos.x, screen_pos.y,  static_cast<int>(health), 8};
    Color color{255, 50, 50, 255};
    graphics.draw(outline, color, false);
    graphics.draw(fill, color, true);

}
#include "player.h"
#include "world.h"

Player::Player(int x, int y, int size)
    :bounding_box{x, y, size, size}, vx{0}, vy{0} {}

void Player::handle_input(const SDL_Event& event) {
    int speed = 10;
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;

        if (key == SDLK_DOWN) {
            vy  = speed;
        }
        else if (key == SDLK_UP) {
            vy = -speed;
        }
        else if (key == SDLK_LEFT) {
            vx = -speed;
        }
        else if (key == SDLK_RIGHT) {
            vx = speed;
        }
    }
}

void Player::update(World& world) {
    // predict the player's future position: make copy of bounding box, move it
    // test for a collision and conditionally move the player's position
    // you can optionally set the velocity back to zero
    SDL_Rect future = bounding_box;
    future.x += vx;
    future.y += vy;

    if (!world.has_any_intersections(future)) {
        bounding_box.x = future.x;
        bounding_box.y = future.y;
    }
    vx = 0;
    vy = 0;
}

std::pair<SDL_Rect, Color> Player::get_sprite() const {
    return {bounding_box, {255, 0, 0, 255}};
}

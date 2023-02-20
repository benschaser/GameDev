#include "world.h"

World::World(int width, int height)
    :tilemap{width, height};

void World::add_platform(int x, int y, int width, int height) {
    for (int n = 0; n <= height; ++n) {
        for (int m = 0; m <= width; ++m) {
            tilemap(x+m, y+n) = Tile::Platform;
        }
    }
}

const std::vector<SDL_Rect>& World::get_platforms() const {
    return platforms;
}

bool World::has_any_intersections(const SDL_Rect& bounding_box) const {
    // test if the given bounding_box intersects any of the platforms
    bool collides = std::any_of(platforms.begin(), platforms.end(), 
                                [&](const SDL_Rect& platform) { // inplace lambda funct
                                    return SDL_HasIntersection(&platform, &bounding_box);
                                });
    return collides;
}
 

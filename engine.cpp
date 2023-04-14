#include "engine.h"
#include "settings.h"
#include "player.h"
#include "level.h"
#include <chrono>

// Engine::Engine(const Settings& settings)
//     :graphics{settings.title, settings.screen_width, settings.screen_height},
//     camera{graphics, settings.tilesize},
//     world{31, 11} {
//         graphics.load_spritesheet(settings.characters);
//         audio.load_sounds(settings.sounds);
//         audio.play_sound("background", true);
//         load_level();
//     }

Engine::Engine(const Settings& settings)
    : graphics{settings.title, settings.screen_width, settings.screen_height},
      camera{graphics, settings.tilesize} {
    
    load_level(settings.starting_level);
}

// void Engine::load_level() {
//     // bounds
//     world.add_platform(0, 0, 30, 1);
//     world.add_platform(0, 0, 1, 10);
//     world.add_platform(30, 0, 1, 10);
//     world.add_platform(0, 10, 31, 1);

//     // platforms
//     world.add_platform(3, 7, 4, 1);
//     world.add_platform(12, 3, 6, 1);

//     // add player
//     player = std::make_shared<Player>(*this, Vec<double>{10, 4}, Vec<int>{1, 1});
//     // move camera
//     camera.move_to(player->physics.position);
// }

void Engine::load_level(const std::string& level_filename) {
    Level level{level_filename, graphics, audio};
    // audio.play_sound("background", true);
    world = std::make_shared<World>(level);

    // load player
    player = std::make_shared<Player>(*this, level.player_start_pos, Vec<int>{1, 1});

    // move camera to start position
    camera.move_to(player->physics.position);
}

void Engine::input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // handle windows and systems events first
        if (event.type == SDL_QUIT) {
            running = false;
            break;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
            grid_on = !grid_on;
        } 
        // pass the rest of the events to the player who will
        // react to keypresses by moving
        auto command = player->handle_input(event, *this);
        if (command) {
            command->execute(*player, *this);
        }
    }
}

void Engine::update(double dt) {
    player->update(*this, dt);
    camera.move_to(player->get_sprite().first);
    camera.update(dt);
}

void Engine::render() {
    graphics.clear();
    camera.render(world->backgrounds);
    camera.render(world->tilemap, grid_on);
    auto [position, color] = player->get_sprite();
    // camera.render(position, color);
    camera.render(position, player->sprite);
    camera.render(*player);
    graphics.update();
}

void Engine::run() {
    running = true;
    audio.play_sound("background", true);
    auto previous = std::chrono::high_resolution_clock::now();
    double lag{0.0};
    while (running) {
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = current - previous;
        previous = current;
        lag += elapsed.count();

        constexpr double dt = 1.0/60.0;

        input();
        while(lag >= dt) {
            update(dt);
            lag -= dt;
        }
        render();
    }
}

void Engine::stop() {
    running = false;
}
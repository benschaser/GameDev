#include "engine.h"
#include "settings.h"
#include "player.h"
#include "level.h"
#include "combat.h"
#include "loadscreen.h"
#include <chrono>
#include <iostream>

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
    for (auto enemy : world->enemies) {
        auto command = enemy->next_action(*this);
        if (command) {
            command->execute(*enemy, *this);
        }
    }
}

void Engine::update(double dt) {
    player->update(*this, dt);
    camera.move_to(player->get_sprite().first);
    camera.update(dt);


    // for (auto enemy : world->enemies) {
    //     if (!enemy.combat.is_alive) {
    //     }
    // }
    for (auto enemy : world->enemies) {
        auto command = enemy->update(*this, dt);
        if (command) {
            command->execute(*enemy, *this);
        }
    }
    for (auto& projectile : world->projectiles) {
        projectile.update(*this, dt);
    }

    // handle collisions between player and enemy
    world->build_quadtree();
    AABB player_box{player->physics.position, {1.0 * player->size.x, 1.0 * player->size.y}};
    std::vector<Entity*> enemies = world->quadtree.query_range(player_box);
    if (enemies.size() > 0) {
        auto enemy = enemies.front();
        enemy->combat.attack(*player);
        // enter hurting state
        // std::cout << player->combat.health << '\n';
        player->state->exit(*player, *this);
        player->state = std::make_unique<Hurting>();
        player->state->enter(*player, *this);
    }

    for (auto & projectile : world->projectiles) {
        AABB p_box{projectile.physics.position, {1.0*projectile.size.x, 1.0*projectile.size.y}};
        std::vector<Entity*> entities = world->quadtree.query_range(p_box);
        for (auto entity : entities) {
            projectile.combat.attack(*entity);
        }
    }

    if (!player->combat.is_alive) {
        EndGame endgame;
        endgame.execute(*player, *this);
        return;
    }

    // check for deaths
    world->remove_inactive();
    // world->enemies.erase(std::remove_if(world->enemies.begin(),world->enemies.end(), [](std::shared_ptr<Enemy>enemy){return !enemy->combat.is_alive;}), world->enemies.end());
}

void Engine::render() {
    graphics.clear();
    camera.render(world->backgrounds);
    camera.render(world->tilemap, grid_on);
    auto [position, color] = player->get_sprite();
    // camera.render(position, color);
    camera.render(position, player->sprite);
    camera.render(*player);
    for (auto enemy : world->enemies) {
        camera.render(*enemy);
    }
    for (auto& projectile : world->projectiles) {
        camera.render(projectile);
    }
    camera.render_life(player->combat.health, player->combat.max_health);
    graphics.update();
}

void Engine::run() {
    running = true;
    audio.play_sound("background", true);
    auto previous = std::chrono::high_resolution_clock::now();
    double lag{0.0};
    while (running) {
        if (next_level) {
            load_level(next_level.value());
            audio.play_sound("background", true);
            next_level.reset();
        }
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
    setup_end_screen();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {  // closing the window
                running = false;
                break;
            }
        }
        graphics.clear();
        camera.render(world->backgrounds);
        graphics.update();
    }
}

void Engine::stop() {
    running = false;
}

void Engine::setup_end_screen() {
    running = true;
    Loadscreen game_over{"assets/game-over.txt", graphics, audio};
    world->backgrounds = game_over.backgrounds;
}
#include "engine.h"
#include "settings.h"
#include "player.h"
#include "level.h"
#include "combat.h"
#include "loadscreen.h"
#include <chrono>
#include <iostream>

Engine::Engine(const Settings& settings)
    : graphics{settings.title, settings.screen_width, settings.screen_height},
      camera{graphics, settings.tilesize} {
    
    load_level(settings.starting_level);
}

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
            window_open = false;
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
            // if (command)
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
    if (win) {
        running = false;
    }
    player->update(*this, dt);
    camera.move_to(player->get_sprite().first);
    camera.update(dt);
    camera.update_tiles(world->tilemap, dt);

    for (auto enemy : world->enemies) {
        auto command = enemy->update(*this, dt);
        if (command) {
            command->execute(*enemy, *this);
        }
    }
    for (auto& projectile : world->projectiles) {
        projectile.update(*this, dt);
        Vec<double> pos = projectile.physics.position;
        if (pos.x <= 2 || pos.y <= 2 || pos.x >= graphics.level_width - 2 || pos.y >= graphics.level_height - 2) {
            // remove projectile
            projectile.combat.is_alive = false;
        }
    }

    // handle collisions between player and enemy
    world->build_quadtree();
    AABB player_box{player->physics.position, {1.0 * player->size.x, 1.0 * player->size.y}};
    std::vector<Entity*> enemies = world->quadtree.query_range(player_box);
    if (enemies.size() > 0) {
        auto enemy = enemies.front();
        if (enemy->combat.is_alive && !player->grounded && player->combat.is_alive) {
            enemy->combat.attack(*player);
            // enter hurting state
            player->state->exit(*player, *this);
            player->state = std::make_unique<Hurting>();
            player->state->enter(*player, *this);
        }
        else if (enemy->combat.is_alive && player->combat.is_alive) {
            player->combat.attack(*enemy);
            player->physics.velocity = {2, 2};
        }
        
    }

    for (auto & projectile : world->projectiles) {
        AABB p_box{projectile.physics.position, {1.0*projectile.size.x, 1.0*projectile.size.y}};
        std::vector<Entity*> entities = world->quadtree.query_range(p_box);
        entities.erase(std::remove_if(entities.begin(), entities.end(), [](Entity* entity){return !entity->combat.is_alive;}), entities.end());
        for (auto entity : entities) {
            if (entity->combat.is_alive) {
                projectile.combat.attack(*entity);
            }
            
            if (entity->physics.velocity.y == 0) {
                entity->physics.velocity.y = 2;
            }
            if (projectile.sprite.flip == false) {
                entity->physics.velocity.x = 2;
            }
            else {
                entity->physics.velocity.x = -2;
            }
        }
        
        if (entities.size() > 0) {
            projectile.hit_enemy = true;
        }
    }

    // check for deaths
    world->remove_inactive();
}

void Engine::render(double dt) {
    graphics.clear();
    camera.render(world->backgrounds);
    camera.render(world->tilemap, grid_on);
    
    for (auto enemy : world->enemies) {
        camera.render(*enemy);
        if (enemy->combat.is_alive) {
            camera.render_enemy_health(*enemy);
        }
    }
    auto [position, color] = player->get_sprite();
    camera.render(position, player->sprite);
    camera.render(*player);
    for (auto& projectile : world->projectiles) {
        camera.render(projectile);
    }
    camera.render_life(player->combat.health, player->combat.max_health);
    graphics.update();
}

void Engine::run() {
    running = true;
    window_open = true;
    audio.play_sound("background", true);
    auto previous = std::chrono::high_resolution_clock::now();
    double lag{0.0};
    while (running) {
        if (next_level) {
            load_level(next_level.value());
            audio.play_sound("background", true);
            audio.play_sound("teleport");
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
        
        render(dt);
        
    }
    
    if (window_open) {
        audio.stop_background();
        audio.stop_sound();
        Sprite bkg = graphics.get_sprite("space");
        Sprite words1 = graphics.get_sprite("game_won");
        Sprite words2 = graphics.get_sprite("game_lost");
        if (win) {
            audio.play_sound("game_won");
        }
        else {
            audio.play_sound("game_over");
        }
        while (window_open) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {  // closing the window
                    window_open = false;
                    break;
                }
            }
            camera.render_screen({640, 720}, bkg);
            if (win) {
                camera.render_screen({640, 840}, words1);
            }
            else {
                camera.render_screen({640, 840}, words2);
            }
            graphics.update();
        }
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
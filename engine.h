#pragma once

#include <memory>
#include <optional>
#include "world.h"
#include "camera.h"
#include "graphics.h"
#include "audio.h"
#include "combat.h"

class Player;
class Settings;

class Engine {
public:
    Engine(const Settings& settings);

    void load_level();
    void load_level(const std::string& level_filename);
    void run();
    void stop();

    Graphics graphics;
    Camera camera;
    std::shared_ptr<World> world;
    Audio audio;
    std::shared_ptr<Player> player;
    std::optional<std::string> next_level;
    bool win{false};
private:
    bool running{true};
    bool window_open{true};
    bool grid_on{false};
    bool game_over{false};
    

    void input();
    void update(double dt);
    void render(double dt);
    void setup_end_screen();
};
#pragma once

#include <memory>
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
private:
    bool running{true};
    bool grid_on{false};

    void input();
    void update(double dt);
    void render();
};
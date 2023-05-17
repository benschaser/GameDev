#include "level.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "graphics.h"
#include "audio.h"

Level::Level(const std::string filename, Graphics& graphics, Audio& audio)
    :filename{filename} {
        load(graphics, audio);
    }

void Level::load(Graphics& graphics, Audio& audio) {
    std::ifstream input{filename};
    // error if can't open
    if (!input) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    // load level's theme
    std::string theme_filename;
    input >> theme_filename;
    load_theme(theme_filename, graphics, audio);

    // read level layout
    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line);) {
        if (line.empty()) continue;
        lines.push_back(line);
    }

    // set level dimensions
    height = lines.size();
    width = lines.front().size();
    graphics.level_height = height;
    graphics.level_width = width;

    // ensure rectangular level
    bool rectangular = std::all_of(std::begin(lines), std::end(lines),
        [=](const std::string& line) {
            return static_cast<int>(line.size()) == width;
        });
    // error handling
    if (!rectangular) {
        throw std::runtime_error("Level is not rectangular");
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            char symbol = lines.at(y).at(x);
            if (std::isspace(symbol)) continue;

            if (symbol == 'P') { // player start
                player_start_pos = Vec<double>{static_cast<double>(x), static_cast<double>(height - y - 1)};
                continue;
            }

            // determine tiletype
            auto it = tile_types.find(symbol);
            auto eit = enemy_types.find(symbol);
            if (it != tile_types.end()) { // found
                Vec<int> position{x, height - y - 1};
                const Tile& tile = it->second;
                tiles.push_back({position, tile});
            }
            else if (eit != enemy_types.end()) {
                Vec<double> position{static_cast<double>(x), static_cast<double>(height - 1 - y)};
                const EnemyType& type = eit->second(graphics);
                enemies.push_back({position, type});
            }
            else {
                // error handle for player starting pos = (-1, -1)
            }
        }
    }
}

void Level::load_theme(const std::string& filename, Graphics& graphics, Audio& audio) {
    std::ifstream input{"assets/" + filename};

    if (!input) {
        throw std::runtime_error("Unable to open " + filename);
    }
    
    // nicely formatted error message
    auto error_message = [](std::string errfilename, int line_num, std::string message, std::string line) {
        return errfilename + ":" + std::to_string(line_num) + " " + message + ": " + line;
    };
    
    int line_num{1};
    for (std::string line; std::getline(input, line); ++line_num) {
        if (line.empty()) {
            continue;
        }
        std::stringstream ss{line};
        std::string command;
        ss >> command;
        if (command == "load-spritesheet") {
            std::string filename;
            ss >> filename;
            if (!ss) {
                std::string msg = error_message(filename, line_num, "Unable to load spritesheet", line);
                throw std::runtime_error(msg);
            }
            graphics.load_spritesheet("assets/" + filename);
        } else if (command == "load-sounds") {
            std::string filename;
            ss >> filename;
            if (!ss) {
                std::string msg = error_message(filename, line_num, "Unable to load sounds", line);
                throw std::runtime_error(msg);
            }
            audio.load_sounds("assets/" + filename);
        } else if (command == "background") {
            std::string filename;
            int scale, distance;
            ss >> filename >> scale >> distance;
            if (!ss) {
                std::string msg = error_message(filename, line_num, "Unable to load background", line);
                throw std::runtime_error(msg);
            }
            Sprite background = graphics.load_image("assets/" + filename);
            background.scale = scale;
            backgrounds.push_back({background, distance});
        } else if (command == "enemy") {
            char symbol;
            std::string type_name;
            // double acceleration;
            ss >> symbol >> type_name;
            if (!ss) {
                throw std::runtime_error("Unable to load enemy");
            }
            auto generate_enemy_type = [=](Graphics& graphics) {return create_enemytype(graphics, type_name);};
            enemy_types[symbol] = generate_enemy_type;
        } else if (command == "tile") {
            char symbol;
            std::string sprite_name;
            bool blocking;
            ss >> symbol >> sprite_name >> std::boolalpha >> blocking;
            if (!ss) {
                std::string msg = error_message(filename, line_num, "Unable to load tile", line);
                throw std::runtime_error(msg);
            }
            AnimatedSprite sprite = graphics.get_animated_sprite(sprite_name, 0.1);
            Tile tile{sprite, blocking};
            

            // read possible commands
            std::string command_name;
            if (ss >> command_name) {
                std::vector<std::string> arguments;
                for (std::string argument; ss >> argument;) {
                    arguments.push_back(argument);
                }
                tile.command = create_command(command_name, arguments);
            }
            tile_types[symbol] = tile;
        } else {
            std::string msg = error_message(filename, line_num, "Unknown command", line);
            throw std::runtime_error(msg);
        }
    }
}
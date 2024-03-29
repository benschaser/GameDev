#include "settings.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

Settings::Settings(const std::string& filename)
    :filename{filename} {
        load();
    }

void Settings::load() {
    std::ifstream input{filename};
    if (!input) {
        throw std::runtime_error("Cannot open settings file: " + filename);
    }
    for (std::string key, value; input >> key >> value;) {
        parameters[key] = value;
    }
    load("title", title);
    load("screen_width", screen_width);
    load("screen_height", screen_height);
    load("tilesize", tilesize);
    load("starting_level", starting_level);
}

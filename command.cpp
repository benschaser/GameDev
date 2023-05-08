#include "command.h"
#include "player.h"
#include "engine.h"

//////////////////
// Stop
//////////////////
void Stop::execute(Entity& player, Engine&) {
    // player.color = {255, 0, 0, 255};
    player.physics.velocity.y = 0.0;
    player.physics.acceleration.x = 0.0;
}

//////////////////
// Accelerate
//////////////////
Accelerate::Accelerate(double acceleration)
    :acceleration{acceleration} {}

void Accelerate::execute(Entity& player, Engine&) {
    // player.physics.velocity.y = 0.0;
    player.physics.acceleration.x = acceleration;
}

//////////////////
// Jump
//////////////////
Jump::Jump(double velocity)
    :velocity{velocity} {}

void Jump::execute(Entity& player, Engine&) {
    player.physics.acceleration.x = 0;
    player.physics.velocity.y = velocity;
    // engine.audio.play_sound("jumping");
    // put this in fsm - put Engine& engine in parameters of every enter() funct
}

//////////////////
// GroundPound
//////////////////
GroundPound::GroundPound() {}

void GroundPound::execute(Entity& player, Engine&) {
    player.physics.velocity.x = 0;
    player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// Dive
//////////////////
Dive::Dive(double vx)
    :vx{vx} {}

void Dive::execute(Entity& player, Engine&) {
    player.physics.velocity.x = vx;
    player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// Fire
//////////////////
Fire::Fire(Projectile projectile, Vec<double> position, Vec<double> velocity)
    :projectile{projectile} {
        this->projectile.physics.position = position;
        this->projectile.physics.velocity = velocity;
        this->projectile.physics.acceleration.y = -gravity;
    }

void Fire::execute(Entity&, Engine& engine) {
    engine.world->projectiles.push_back(projectile);
}

//////////////////
// Game Changes
//////////////////
void EndGame::execute(Entity&, Engine& engine) {
    engine.stop();
}

PlaySound::PlaySound(std::string sound_name, bool is_background)
    :sound_name{sound_name}, is_background{is_background} {}

void PlaySound::execute(Entity&, Engine& engine) {
    engine.audio.play_sound(sound_name, is_background);
}

LoadLevel::LoadLevel(const std::string& filename)
    :filename{filename} {}

void LoadLevel::execute(Entity&, Engine& engine) {
    engine.audio.stop_background();
    engine.next_level = "assets/" + filename;
}

std::shared_ptr<Command> create_command(std::string command_name, std::vector<std::string> arguments) {
    if (command_name == "end_game") {
        return std::make_shared<EndGame>();
    }
    else if (command_name == "play_sound") {
        bool is_background = arguments.at(1) == "true" ? true : false;
        std::string sound_name = arguments.at(0);
        return std::make_shared<PlaySound>(sound_name, is_background);
    }
    else if (command_name == "load_level") {
        if (arguments.size() != 1) {
            throw std::runtime_error("Too many arguments to load level");
        }
        return std::make_shared<LoadLevel>(arguments.front());
    }

    throw std::runtime_error("Unkown command: " + command_name);
}


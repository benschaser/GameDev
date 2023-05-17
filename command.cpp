#include "command.h"
#include "player.h"
#include "engine.h"
#include <randomness.h>

//////////////////
// Stop
//////////////////
void Stop::execute(Entity& player, Engine&) {
    player.physics.velocity.y = 0.0;
    player.physics.acceleration.x = 0.0;
}

//////////////////
// Accelerate
//////////////////
Accelerate::Accelerate(double acceleration)
    :acceleration{acceleration} {}

void Accelerate::execute(Entity& player, Engine&) {
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
Fire::Fire(const Player& player) {
    Vec<double> position{player.physics.position.x + player.size.x, player.physics.position.y + (player.size.y / 2) + 0.25};
    Vec<double> velocity{30, 0.0};
    if (player.sprite.flip) {
        position = {player.physics.position.x - player.size.x, player.physics.position.y + (player.size.y / 2) + 0.25};
        velocity.x *= -1;
    }
    position.y += randint(-1, 1) * 0.1;
    projectile = player.projectile;
    projectile.physics.position = position;
    projectile.physics.velocity = velocity;
}

void Fire::execute(Entity& entity, Engine& engine) {
    projectile.physics.clamp_velocity = false;
    projectile.combat.attack_damage = entity.combat.attack_damage;
    if (entity.sprite.flip) {
        projectile.anim_sprite.flip(true);
        projectile.wall_impact_sprite.flip(true);
        projectile.enemy_impact_sprite.flip(true);
    }
    engine.audio.play_sound("firing");
    engine.world->projectiles.push_back(projectile);
}

//////////////////
// EnemyHurt
//////////////////
EnemyHurt::EnemyHurt() {}

void EnemyHurt::execute(Entity&, Engine&) {
    // entity.sprite = entity.hurt_sprite;
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
    // engine.audio.stop_background();
    engine.next_level = "assets/" + filename;
}

void Powerup::execute(Entity& entity, Engine& engine) {
    // player powerup sound
    entity.carrying = true;
    entity.gun_level += 1;
    engine.audio.play_sound("powerup");
}

void WinGame::execute(Entity& entity, Engine& engine) {
    engine.win = true;
}

std::shared_ptr<Command> create_command(std::string command_name, std::vector<std::string> arguments) {
    if (command_name == "end_game") {
        return std::make_shared<EndGame>();
    }
    else if (command_name == "win_game") {
        return std::make_shared<WinGame>();
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
    else if (command_name == "powerup") {
        return std::make_shared<Powerup>();
    }

    throw std::runtime_error("Unkown command: " + command_name);
}


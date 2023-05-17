#pragma once
#include <memory>
#include <vector>
#include <string>
#include "projectile.h"

class Entity;
class Player;
class Engine;

class Command {
public:
    virtual ~Command() {}
    virtual void execute(Entity& player, Engine& engine) = 0;
};

class Stop : public Command {
public:
    void execute(Entity& player, Engine& engine) override;
};

class Accelerate : public Command {
public:
    Accelerate(double acceleration);
    void execute(Entity& player, Engine& engine) override;
private:
    double acceleration;
};

class Jump : public Command {
public:
    Jump(double velocity);
    void execute(Entity& player, Engine& engine) override;
private:
    double velocity;
};

class GroundPound : public Command {
public:
    GroundPound();
    void execute(Entity& player, Engine& engine) override;
};

class Dive : public Command {
public:
    Dive(double vx);
    void execute(Entity& player, Engine& engine) override;
private:
    double vx;
};

class EnemyHurt : public Command {
public:
    EnemyHurt();
    void execute(Entity& player, Engine& engine) override;
};

class Fire : public Command {
public:
    // Fire(Projectile projectile, Vec<double> position, Vec<double> velocity);
    Fire(const Player& player);
    void execute(Entity& player, Engine& engine) override;
private:
    Projectile projectile;
};

class EndGame : public Command {
public:
    void execute(Entity& player, Engine& engine) override;
};

class PlaySound : public Command {
public:
    PlaySound(std::string sound_name, bool is_background);
    void execute(Entity& player, Engine& engine) override;
private:
    std::string sound_name;
    bool is_background;
};

class LoadLevel : public Command {
public:
    LoadLevel(const std::string& filename);
    void execute(Entity& player, Engine& engine) override;
private:
    std::string filename;
};

class Powerup : public Command {
public:
    void execute(Entity& player, Engine& engine) override;
};

class WinGame : public Command {
public:
    void execute(Entity& player, Engine& engine) override;
};

std::shared_ptr<Command> create_command(std::string command_name, std::vector<std::string> arguments);
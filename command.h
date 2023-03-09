#pragma once

class Player;
class World;

class Command {
public:
    virtual ~Command() {}
    virtual void execute(Player& player, World& world) = 0;
};

class Stop : public Command {
public:
    void execute(Player& player, World& world) override;
};

class Accelerate : public Command {
public:
    Accelerate(double acceleration);
    void execute(Player& player, World& world) override;
private:
    double acceleration;
};

class Jump : public Command {
public:
    Jump(double velocity);
    void execute(Player& player, World& world) override;
private:
    double velocity;
};

// class MoveInAir : public Command {
// public:
//     MoveInAir(double acceleration);
//     void execute(Player& player, World& world) override;
// private:
//     double acceleration;
// };

class GroundPound : public Command {
public:
    GroundPound();
    void execute(Player& player, World& world) override;
};

// class MoveInAir : public Command {
// public:
//     MoveInAir(double acceleration);
//     void execute(Player& player, World& world) override;
// private:
//     double acceleration;
// };

class Dive : public Command {
public:
    Dive(double vx);
    void execute(Player& player, World& world) override;
private:
    double vx;
};
#pragma once

#include <SDL2/SDL.h>
#include <memory>

class Player;
class World;

class State {
public:
    virtual ~State() {}
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) = 0;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt);
    virtual void enter(Player&) {}
    virtual void exit(Player&) {}
};

class Standing : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
    // virtual void exit(Player&) {}
};

class Walking : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
    // virtual void exit(Player&) {}
};

// class InAirWalking : public State {
// public:
//     virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
//     virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
//     virtual void enter(Player& player) override;
//     // virtual void exit(Player&) {}
// };

// class Falling : public State {
// public:
//     virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
//     virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
//     virtual void enter(Player& player) override;
//     // virtual void exit(Player&) {}
// };

class InAir : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
};

class Jumping : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
    // virtual void exit(Player&) {}
};

class GroundPounding : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
    // virtual void exit(Player&) {}
};

class Diving : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, World& world, double dt) override;
    virtual void enter(Player& player) override;
    // virtual void exit(Player&) {}
};

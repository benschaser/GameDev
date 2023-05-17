#pragma once

#include <SDL2/SDL.h>
#include <memory>

class Player;
class Engine;

class State {
public:
    virtual ~State() {}
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) = 0;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt);
    virtual void enter(Player&, Engine&) {}
    virtual void exit(Player&, Engine&) {}
};

class Standing : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
};

class Walking : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
    virtual void exit(Player&, Engine& engine) override;
};

class InAir : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
};

class Jumping : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
};

class GroundPounding : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
    virtual void exit(Player&, Engine& engine) override;
};

class Diving : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
};

class AttackAll : public State {
public:
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual void enter(Player& player, Engine& engine) override;
};

class Hurting : public State {
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
    virtual void exit(Player& player, Engine& engine) override;

    double cooldown = 0.2;
    double elapsed_time = 0.0;
};

class Dying : public State {
    virtual std::unique_ptr<State> handle_input(Player& player, const SDL_Event& event) override;
    virtual std::unique_ptr<State> update(Player& player, Engine& engine, double dt) override;
    virtual void enter(Player& player, Engine& engine) override;
    virtual void exit(Player& player, Engine& engine) override;

    double cooldown = 2.0;
    double elapsed_time = 0.0;
};
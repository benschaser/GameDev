#include "command.h"
#include "player.h"
#include "world.h"

//////////////////
// Stop
//////////////////
void Stop::execute(Player& player, World& world) {
    player.color = {255, 0, 0, 255};
    player.physics.velocity.y = 0.0;
    player.physics.acceleration.x = 0.0;
}

//////////////////
// Accelerate
//////////////////
Accelerate::Accelerate(double acceleration)
    :acceleration{acceleration} {}

void Accelerate::execute(Player& player, World& world) {
    player.color = {255, 255, 0, 255};
    // player.physics.velocity.y = 0.0;
    player.physics.acceleration.x = acceleration;
}

//////////////////
// Jump
//////////////////
Jump::Jump(double velocity)
    :velocity{velocity} {}

void Jump::execute(Player& player, World& world) {
    player.color = {0, 0, 255, 255};
    player.physics.acceleration.x = 0;
    player.physics.velocity.y = velocity;
}

// //////////////////
// // MoveInAir
// //////////////////
// MoveInAir::MoveInAir(double acceleration)
//     :acceleration{acceleration} {}

// void MoveInAir::execute(Player& player, World& world) {
//     player.color = {255, 0, 255, 255};
//     player.physics.velocity.y += 20;
// }

//////////////////
// GroundPound
//////////////////
GroundPound::GroundPound() {}

void GroundPound::execute(Player& player, World& world) {
    player.color = {255, 0, 255, 255};
    player.physics.velocity.y = groundpound_velocity;
}

//////////////////
// Dive
//////////////////
Dive::Dive(double vx)
    :vx{vx} {}

void Dive::execute(Player& player, World& world) {
    player.color = {0, 255, 255, 255};
    player.physics.velocity.y = groundpound_velocity;
}
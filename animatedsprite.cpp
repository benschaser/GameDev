#include "animatedsprite.h"
#include <iostream>

AnimatedSprite::AnimatedSprite(const std::vector<Sprite> &sprites, double dt_per_frame, int starting_frame)
    : sprites{sprites}, dt_per_frame{dt_per_frame}, time{0}, current_frame{starting_frame} {
        total_frames = sprites.size();
    }

void AnimatedSprite::flip(bool flip) {
    for (auto& sprite : sprites) {
        sprite.flip = flip;
    }
}

void AnimatedSprite::update(double dt) {
    time += dt;
    if (time >= dt_per_frame) {
        time -= dt_per_frame;
        if (sprites.size() > 0) {
            current_frame = (current_frame + 1);
            if (loop) {
                current_frame %= total_frames;
            }
            else {
                if (current_frame >= 8) {
                    current_frame = 7;
                }
            }
            
        }
    }
}

void AnimatedSprite::reset() {
    time = 0;
    current_frame = 0;
}

Sprite AnimatedSprite::get_sprite() const {
    if (sprites.size() > 0) {
        return sprites.at(current_frame);
    }
    else {
        return Sprite();
    }
    
}

int AnimatedSprite::number_of_frames() const {
    return sprites.size();
}

void AnimatedSprite::rotate(int degrees) {
    for (auto& sprite : sprites) {
        sprite.angle = degrees;
    }
}
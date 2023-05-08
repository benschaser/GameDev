#pragma once

#include "vec.h"

class Sprite {
public:
    int texture_id{-1};
    Vec<int> location{0, 0}; // upper left corner in img file
    Vec<int> size{0, 0}; // size in img
    int scale{1};
    
    // display properties
    Vec<int> shift{0, 0};
    Vec<int> center{0, 0};
    double angle{0.0};
    bool flip{false}; // flip for walking opposite directions
};
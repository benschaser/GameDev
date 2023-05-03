#pragma once

#include <memory>
#include <vector>
#include "vec.h"

class Entity;

// rectangular AABB
struct AABB {
    Vec<double> center, half_dimension;

    bool contains(const Vec<double>& point) const;
    bool intersects(const AABB& other) const;
};

class QuadTree {
public:
    QuadTree(AABB boundary);

    void clear();
    void subdivide();
    std::vector<Entity*> query_range(AABB range) const;

    bool insert(Entity* object);
    static constexpr std::size_t NODE_CAPACITY = 4;

    AABB boundary;
    std::vector<Entity*> objects;
    std::shared_ptr<QuadTree> nw, ne, sw, se;
};
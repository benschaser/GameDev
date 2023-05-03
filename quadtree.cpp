#include "quadtree.h"
#include "entity.h"
#include <algorithm>

bool AABB::contains(const Vec<double>& point) const {
    Vec<double> displacement = point - center;
    return std::abs(displacement.x) < half_dimension.x
        && std::abs(displacement.y) < half_dimension.y;
}

bool AABB::intersects(const AABB& other) const {
    Vec<double> displacement = other.center - center;
    return std::abs(displacement.x) < (other.half_dimension.x + half_dimension.x)
        && std::abs(displacement.y) < (other.half_dimension.y + half_dimension.y);
}

QuadTree::QuadTree(AABB boundary)
    :boundary{boundary} {}

void QuadTree::clear() {
    objects.clear();
    if (nw) {
        nw = nullptr;
    }
    if (ne) {
        ne = nullptr;
    }
    if (sw) {
        sw = nullptr;
    }
    if (se) {
        se = nullptr;
    }
}

bool QuadTree::insert(Entity* object) {
    // ignore objects that don't belong
    if (!boundary.contains(object->physics.position)) {
        return false;
    }

    // if there is space and no children then object is stored
    if (objects.size() < NODE_CAPACITY && nw == nullptr) {
        objects.push_back(object);
        return true;
    }

    // otherwise subdivide and insert into children
    if (nw == nullptr) {
        subdivide();
    }

    return nw->insert(object) || ne->insert(object) || sw->insert(object) || se->insert(object);
}

void QuadTree::subdivide() {
    Vec<double> half = boundary.half_dimension / 2.0;

    // nw = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y + half.y}, half});
    // ne = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y + half.y}, half});
    // sw = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y - half.y}, half});
    // nw = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y - half.y}, half});

    nw = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y + half.y}, half});
    ne = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y + half.y}, half});
    sw = std::make_shared<QuadTree>(AABB{{boundary.center.x - half.x, boundary.center.y - half.y}, half});
    se = std::make_shared<QuadTree>(AABB{{boundary.center.x + half.x, boundary.center.y - half.y}, half});


    for (Entity* object : objects) {
        insert(object);
    }
    objects.clear();
}

std::vector<Entity*> QuadTree::query_range(AABB range) const {
    if (!boundary.intersects(range)) {
        return {};
    }

    // handle leaf nodes
    std::vector<Entity*> results;
    if (nw == nullptr) {
        std::copy_if(objects.begin(), objects.end(), std::back_inserter(results), [&](Entity* object) {
                        return range.contains(object->physics.position);
                    });

        return results;
    }

    // handle parent nodes
    // add objects from children
    auto pts = nw->query_range(range);
    results.insert(results.end(), pts.begin(), pts.end());
    pts = ne->query_range(range);
    results.insert(results.end(), pts.begin(), pts.end());
    pts = sw->query_range(range);
    results.insert(results.end(), pts.begin(), pts.end());
    pts = se->query_range(range);
    results.insert(results.end(), pts.begin(), pts.end());

    return results;
}

#include "Collision.hpp"
#include <algorithm>

float IntersectsAABB(const AABB& a, const AABB& b) {
    float dx = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    float dy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    float dz = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);
    float overlap = std::min({dx, dy, dz});
    return overlap;
}

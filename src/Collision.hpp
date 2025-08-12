#pragma once

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

bool IntersectsAABB(const AABB& a, const AABB& b);

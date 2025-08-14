#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>

#include "GameState.hpp"

struct RaycastResult {
    BlockIndex blockPos;
    glm::vec3 hitFace; // Normal of the face that was hit (-1, 0, or 1 for each axis)
    float distance;
};

std::optional<RaycastResult> RaycastBlocks(glm::vec3 pos, glm::vec3 dir, float maxDistance);

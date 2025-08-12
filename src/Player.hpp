#pragma once

#include <glm/glm.hpp>

class Player
{
public:
    enum Movement
    {
        FORWARDS = 1,
        BACKWARDS = 2,
        LEFT = 4,
        RIGHT = 8,
        UP = 16,
        DOWN = 32,
    };

    glm::vec3 Pos;

    Player(glm::vec3 pos);
    void ProcessKeyboardMovement(uint8_t direction);
};

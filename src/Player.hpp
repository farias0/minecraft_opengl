#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Collision.hpp"

class Player
{
public:
    glm::vec3 Pos;

    Player(glm::vec3 pos);
    void ProcessKeyboardMovement(GLFWwindow *window);
    void Update();

    AABB GetCollisionBox();
    AABB GetCollisionBoxFeet();

private:
    bool isFlying = true;
    double lastPressedJump;
    bool isOnGround = false;

    glm::vec3 velocity;

    void ToggleFlyMode();
};

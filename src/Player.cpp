#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Camera.hpp"
#include "Collision.hpp"
#include "Cube.hpp"
#include "Debug.hpp"
#include "GameState.hpp"
#include "Player.hpp"

static const double JUMP_DOUBLE_PRESS_WINDOW = 0.15f;
static const double JUMP_DOUBLE_PRESS_WINDOW_MIN = 0.07f;
static const float PLAYER_HEIGHT = 2.0f;
static const float PLAYER_EYE_LEVEL = 0.85f; // proportionally to height

Player::Player(glm::vec3 pos) :
    Pos(pos)
{

}

void Player::ProcessKeyboardMovement(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        double now = glfwGetTime();
        if (now - lastPressedJump > JUMP_DOUBLE_PRESS_WINDOW_MIN && now - lastPressedJump <= JUMP_DOUBLE_PRESS_WINDOW)
        {
            ToggleFlyMode();
            lastPressedJump = 0;
        }
        lastPressedJump = now;
    }

    glm::vec3 deltaMove = glm::vec3(0, 0, 0);
    glm::vec3 noMove = deltaMove;

    if (IsFlying)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            deltaMove += camera->Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            deltaMove -= camera->Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            deltaMove -= camera->Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            deltaMove += camera->Right;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            deltaMove += WORLD_UP;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            deltaMove -= WORLD_UP;
    }
    else
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            deltaMove += glm::normalize(glm::cross(WORLD_UP, camera->Right));
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            deltaMove -= glm::normalize(glm::cross(WORLD_UP, camera->Right));;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            deltaMove -= camera->Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            deltaMove += camera->Right;
    }

    if (deltaMove != noMove)
            Pos += MOVE_SPEED * deltaTime * glm::normalize(deltaMove);
}

void Player::Update()
{
    if (IsFlying) velocity = glm::vec3(0, 0, 0);
    else
    {
        isOnGround = false;

        // TODO only check for cubes nearby
        for (auto it = cubes.begin(); it != cubes.end(); it++)
        {
            if (IntersectsAABB(GetCollisionBoxFeet(), it->second->GetCollisionBox()) > 0)
            {
                isOnGround = true;
                velocity.y = 0;
                Pos.y = it->second->pos.y + 0.5f + (PLAYER_HEIGHT * 0.5f);
            }
            else if (float intersect = IntersectsAABB(GetCollisionBox(), it->second->GetCollisionBox()); intersect > 0)
            {
                Pos += glm::normalize(Pos - it->second->pos) * intersect;
            }
        }

        if (!isOnGround)
        {
            velocity.y -= GRAVITY * deltaTime;
        }

        Pos += velocity * deltaTime;
    }
}

AABB Player::GetCollisionBox()
{
    AABB box;
    box.min = Pos + glm::vec3(-0.5f,  PLAYER_HEIGHT * -0.5 * (1 - PLAYER_EYE_LEVEL),  -0.5f);
    box.max = Pos + glm::vec3( 0.5f,  PLAYER_HEIGHT *  0.5 * PLAYER_EYE_LEVEL,         0.5f);
    return box;
}

AABB Player::GetCollisionBoxFeet()
{
    AABB box;
    box.min = Pos + glm::vec3(-0.25f,  (PLAYER_HEIGHT * -0.5) - 0.2, -0.25f);
    box.max = Pos + glm::vec3( 0.25f,  (PLAYER_HEIGHT * -0.5) + 0.2,  0.25f);
    return box;
}

void Player::ToggleFlyMode()
{
    IsFlying = !IsFlying;
}

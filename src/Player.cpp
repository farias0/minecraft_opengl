#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Camera.hpp"
#include "Collision.hpp"
#include "Cube.hpp"
#include "Debug.hpp"
#include "GameState.hpp"
#include "Player.hpp"

static const double JUMP_DOUBLE_PRESS_WINDOW = 0.5f;
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
        if (now - lastPressedJump > (deltaTime*5) && now - lastPressedJump <= JUMP_DOUBLE_PRESS_WINDOW)
        {
            ToggleFlyMode();
            lastPressedJump = 0;
        }
        lastPressedJump = now;
    }

    if (isFlying)
    {
        glm::vec3 deltaMove = glm::vec3(0, 0, 0);
        glm::vec3 noMove = deltaMove;

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

        if (deltaMove != noMove)
            Pos += MOVE_SPEED * deltaTime * glm::normalize(deltaMove);
    }
}

void Player::Update()
{
    if (isFlying) velocity = glm::vec3(0, 0, 0);
    else
    {
        if (!isOnGround) // TODO rethink isOnGround
        {
            velocity.y -= GRAVITY * deltaTime;
        }

        bool collision = false;
        glm::vec3 collidedWith;
        const float pushbackDistance = 0.5f;

        // TODO only check for cubes nearby
        for (auto it = cubes.begin(); it != cubes.end(); it++)
        {
            if (IntersectsAABB(GetCollisionBox(), it->second->GetCollisionBox()))
            {
                collidedWith = it->second->pos;
                collision = true;
                break;
            }
        }

        if (collision)
        {
            velocity = glm::normalize(Pos - collidedWith) * pushbackDistance;

            std::stringstream logMsg;
            logMsg << "PLAYER collision with x=" << collidedWith.x << " y=" << collidedWith.y << " z=" << collidedWith.z;
            DebugLog(logMsg.str());
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

void Player::ToggleFlyMode()
{
    isFlying = !isFlying;

    // std::stringstream logMsg;
    // logMsg << "PLAYER flying mode = " << isFlying;
    // DebugLog(logMsg.str());
}

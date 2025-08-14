#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Camera.hpp"
#include "Collision.hpp"
#include "Block.hpp"
#include "Debug.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include "Raycast.hpp"

static const double JUMP_DOUBLE_PRESS_WINDOW = 0.15f;
static const double JUMP_DOUBLE_PRESS_WINDOW_MIN = 0.07f;
static const float PLAYER_HEIGHT = 2.0f;
static const float PLAYER_EYE_LEVEL = 0.85f; // proportionally to height
static const float INTERACT_BLOCK_DIST = 3.5f;
static const float JUMP_IMPULSE = 5.0f;

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
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround)
            Jump();
    }

    if (deltaMove != noMove)
            Pos += MOVE_SPEED * deltaTime * glm::normalize(deltaMove);
}

void Player::ProcessMouseClick(GLFWwindow* window, int button, int action, int mods)
{
    window; mods;

    bool isPuttingBlock = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1;
    bool isRemovingBlock = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_2;

    std::optional<RaycastResult> rayResult = RaycastBlocks(Pos, camera->Front, INTERACT_BLOCK_DIST);

    if (rayResult.has_value())
    {
        BlockIndex blockIndexIndex = rayResult.value().blockPos;

        if (isPuttingBlock)
        {
            glm::vec3 hitFace = rayResult.value().hitFace;
            BlockIndex placeBlockIndex = std::make_tuple(std::get<0>(blockIndexIndex) + (int) hitFace.x, 
                                                            std::get<1>(blockIndexIndex) + (int) hitFace.y,
                                                            std::get<2>(blockIndexIndex) + (int) hitFace.z);
            AddBlock(placeBlockIndex);
        }
        else if (isRemovingBlock)
        {
            RemoveBlock(blockIndexIndex);
        }
    }
    else
    {
        if (isPuttingBlock)
        {
            int x = static_cast<int>(std::round(camera->LookingAt().x));
            int y = static_cast<int>(std::round(camera->LookingAt().y));
            int z = static_cast<int>(std::round(camera->LookingAt().z));
            AddBlock(std::make_tuple(x, y, z));
        }
    }
}

void Player::Update()
{
    if (IsFlying) velocity = glm::vec3(0, 0, 0);
    else
    {
        isOnGround = false;

        // int count = 0;
        // auto start = std::chrono::high_resolution_clock::now();

        for (int cursorX = (int)std::floor(Pos.x); cursorX <= (int)std::ceil(Pos.x); cursorX++)
        {
            for (int cursorY = (int)std::floor(Pos.y) - 1; cursorY <= (int)std::ceil(Pos.y) + 1; cursorY++)
            {
                for (int cursorZ = (int)std::floor(Pos.z); cursorZ <= (int)std::ceil(Pos.z); cursorZ++)
                {
                    auto it = blocks.find(std::make_tuple(cursorX, cursorY, cursorZ));
                    if (it != blocks.end() && it->second != nullptr)
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

                    // count++;
                }
            }
        }

        // auto finish = std::chrono::high_resolution_clock::now();
        // std::stringstream logMsg;
        // logMsg << "PLAYER block collision check count=" << count << ", took us=" << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
        // DebugLog(logMsg.str());

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
    box.min = Pos + glm::vec3(-0.25f,  (PLAYER_HEIGHT * -0.5) - 0.1, -0.25f);
    box.max = Pos + glm::vec3( 0.25f,  (PLAYER_HEIGHT * -0.5) + 0.1,  0.25f);
    return box;
}

void Player::Jump()
{
    Pos += WORLD_UP * 0.1f; // Take off ground
    velocity += WORLD_UP * JUMP_IMPULSE;
}

void Player::ToggleFlyMode()
{
    IsFlying = !IsFlying;
}

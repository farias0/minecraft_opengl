#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Camera.hpp"
#include "Collision.hpp"
#include "Cube.hpp"
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

void Player::ProcessMouseClick(GLFWwindow* window, int button, int action, int mods)
{
    window; mods;

    bool isPuttingBlock = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1;
    bool isRemovingBlock = action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_2;

    std::optional<RaycastResult> rayResult = RaycastBlocks(Pos, camera->Front, INTERACT_BLOCK_DIST);

    if (rayResult.has_value())
    {
        CubeIndex cubeIndex = rayResult.value().blockPos;

        if (isPuttingBlock)
        {
            glm::vec3 hitFace = rayResult.value().hitFace;
            CubeIndex placeCubeIndex = std::make_tuple(std::get<0>(cubeIndex) + hitFace.x, 
                                                        std::get<1>(cubeIndex) + hitFace.y,
                                                        std::get<2>(cubeIndex) + hitFace.z);
            AddCube(placeCubeIndex);
        }
        else if (isRemovingBlock)
        {
            RemoveCube(cubeIndex);
        }
    }
    else
    {
        if (isPuttingBlock)
        {
            int x = static_cast<int>(std::round(camera->LookingAt().x));
            int y = static_cast<int>(std::round(camera->LookingAt().y));
            int z = static_cast<int>(std::round(camera->LookingAt().z));
            AddCube(std::make_tuple(x, y, z));
        }
    }
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

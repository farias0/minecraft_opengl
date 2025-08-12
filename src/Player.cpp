#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>

#include "Camera.hpp"
#include "Debug.hpp"
#include "GameState.hpp"
#include "Player.hpp"

static const double JUMP_DOUBLE_PRESS_WINDOW = 0.5f;

Player::Player(glm::vec3 pos) :
    Pos(pos)
{

}

void Player::ProcessKeyboardMovement(GLFWwindow *window)
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
    {
        deltaMove += WORLD_UP;
        double now = glfwGetTime();
        if (now - lastPressedJump > (deltaTime*5) && now - lastPressedJump <= JUMP_DOUBLE_PRESS_WINDOW)
        {
            ToggleFlyMode();
            lastPressedJump = 0;
        }
        lastPressedJump = now;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        deltaMove -= WORLD_UP;

    if (deltaMove != noMove)
        Pos += MOVE_SPEED * deltaTime * glm::normalize(deltaMove);
}

void Player::Update()
{
    if (!isFlying && !isOnGround)
    {
        velocity.y -= GRAVITY * deltaTime;
        Pos += velocity * deltaTime;
    }
    else velocity = glm::vec3(0, 0, 0);
}

void Player::ToggleFlyMode()
{
    isFlying = !isFlying;

    std::stringstream logMsg;
    logMsg << "PLAYER flying mode = " << isFlying;
    DebugLog(logMsg.str());
}

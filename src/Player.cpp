#include "Camera.hpp"
#include "GameState.hpp"
#include "Player.hpp"

Player::Player(glm::vec3 pos) :
    Pos(pos)
{

}

void Player::ProcessKeyboardMovement(uint8_t direction)
{
    glm::vec3 deltaMove = glm::vec3(0, 0, 0);
    glm::vec3 noMove = deltaMove;
    if (direction & FORWARDS) deltaMove += camera->Front;
    if (direction & BACKWARDS) deltaMove -= camera->Front;
    if (direction & RIGHT) deltaMove += camera->Right;
    if (direction & LEFT) deltaMove -= camera->Right;
    if (direction & UP) deltaMove += WORLD_UP;
    if (direction & DOWN) deltaMove -= WORLD_UP;
    if (deltaMove != noMove)
        Pos += MOVE_SPEED * deltaTime * glm::normalize(deltaMove);
}

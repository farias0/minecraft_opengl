#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

#include "Game.hpp"

#include "Camera.hpp"
#include "Cube.hpp"
#include "GameState.hpp"


std::map<std::tuple<int, int, int>, std::unique_ptr<Cube>> cubes;

bool AddCube(int x, int y, int z);

void StartGame()
{
    camera = std::unique_ptr<Camera>(new Camera());

    AddCube(-2, -1, -5);
    AddCube(-1, -2, -5);
    AddCube(0, -2, -5);
    AddCube(1, -2, -5);
    AddCube(2, -1, -5);
}

void UpdateGame()
{
    //
}

void RenderGame()
{
    for (auto it = cubes.begin(); it != cubes.end(); it++)
    {
        it->second->Render();
    }
}

void ProcessInput(GLFWwindow *window) {

    uint8_t keyboardMove = 0;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        keyboardMove |= Camera::Movement::FORWARDS;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        keyboardMove |= Camera::Movement::BACKWARDS;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        keyboardMove |= Camera::Movement::LEFT;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        keyboardMove |= Camera::Movement::RIGHT;
    if (keyboardMove) camera->ProcessKeyboardMovement(keyboardMove);
}

void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos)
{
    window;

    static double lastX = xPos, lastY = yPos;
    float xOffset = (float)(xPos - lastX);
    float yOffset = (float)(yPos - lastY);
    lastX = xPos;
    lastY = yPos;

    camera->ProcessMouseMovement(xOffset, yOffset);
}

void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    window; xOffset;
    camera->ProcessMouseScroll((float)yOffset);
}

void DebugLog(std::string message)
{
    std::cout << "[DEBUG] " << message;
}

bool AddCube(int x, int y, int z)
{
    std::tuple tuple = std::make_tuple(x, y, z);

    if (cubes[tuple] == nullptr)
    {
        cubes[tuple] = std::unique_ptr<Cube>(new Cube(glm::vec3(x, y, z)));
        return true;
    }
    else
    {
        DebugLog("Can't add cube, space already has one.");
        return false;
    }
}

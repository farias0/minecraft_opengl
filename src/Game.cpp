#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>

#include "Game.hpp"

#include "Camera.hpp"
#include "Cube.hpp"
#include "Debug.hpp"
#include "GameState.hpp"

std::map<std::tuple<int, int, int>, std::unique_ptr<Cube>> cubes;

bool AddCube(int x, int y, int z);
bool RemoveCube(int x, int y, int z);
void GenerateTerrain();

void StartGame()
{
    camera = std::unique_ptr<Camera>(new Camera());
    
    GenerateTerrain();
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

void MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    window; mods;

    int x = static_cast<int>(std::round(camera->LookingAt().x));
    int y = static_cast<int>(std::round(camera->LookingAt().y));
    int z = static_cast<int>(std::round(camera->LookingAt().z));

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1)
    {
        AddCube(x, y, z);
    }
    else if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_2)
    {
        RemoveCube(x, y, z);
    }
}

void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    window; xOffset;
    camera->ProcessMouseScroll((float)yOffset);
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

bool RemoveCube(int x, int y, int z)
{
    std::tuple tuple = std::make_tuple(x, y, z);

    auto it = cubes.find(tuple);
    if (it != cubes.end() && it->second != nullptr)
    {
        cubes.erase(it);
        return true;
    }
    else
    {
        return false;
    }
}

void GenerateTerrain()
{
    const int y = -2;
    int count = 0;
    std::stringstream logMsg;

    logMsg << "TERRAIN GEN started";
    DebugLog(logMsg.str());

    for (int x = -50; x <= 50; x++)
    {
        for (int z = -50; z <= 50; z++)
        {
            AddCube(x, y, z);
            count++;

            // logMsg.str("");
            // logMsg << "TERRAIN GEN added block #" << count << " x=" << x << " y=" << y << " z=" << z;
            // DebugLog(logMsg.str());
        }
    }

    logMsg.str("");
    logMsg << "TERRAIN GEN generated " << count << " blocks.";
    DebugLog(logMsg.str());
}

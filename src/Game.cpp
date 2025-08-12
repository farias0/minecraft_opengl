#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <stb_perlin.h>

#include "Game.hpp"

#include "Camera.hpp"
#include "Cube.hpp"
#include "Debug.hpp"
#include "GameState.hpp"
#include "Player.hpp"

std::map<std::tuple<int, int, int>, std::unique_ptr<Cube>> cubes;

bool AddCube(int x, int y, int z);
bool RemoveCube(int x, int y, int z);
void GenerateTerrain();

void StartGame()
{
    player = std::unique_ptr<Player>(new Player(glm::vec3(0, 0, 0)));

    camera = std::unique_ptr<Camera>(new Camera());
    
    GenerateTerrain();
}

void UpdateGame()
{
    player->Update();
}

void RenderGame()
{
    for (auto it = cubes.begin(); it != cubes.end(); it++)
    {
        it->second->Render();
    }
}

void ProcessInput(GLFWwindow *window) {

    player->ProcessKeyboardMovement(window);
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
    const int surfaceYMax = -2;
    const int terrainThickness = 3;
    const int minX = -15, maxX = 15, minZ = -15, maxZ = 15;
    const float lacunarity = 1.4f, gain = 0.5f;
    const int octaves = 2;
    int count = 0;
    std::stringstream logMsg;

    logMsg << "TERRAIN GEN started";
    DebugLog(logMsg.str());

    for (int x = minX; x <= maxX; x++)
    {
        for (int z = minZ; z <= maxZ; z++)
        {
            float noise = stb_perlin_fbm_noise3((float)x * 0.1f, 0.0f, (float)z * 0.1f, lacunarity, gain, octaves);
            noise = (noise + 1.0f) * 0.5f;
            int columnThickness = static_cast<int>(std::round(terrainThickness * noise));
            for (int y = surfaceYMax - terrainThickness; y < surfaceYMax - terrainThickness + columnThickness; y++)
            {
                AddCube(x, y, z);
                count++;
            }
        }
    }

    logMsg.str("");
    logMsg << "TERRAIN GEN generated " << count << " blocks.";
    DebugLog(logMsg.str());
}

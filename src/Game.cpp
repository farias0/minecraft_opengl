#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <stb_perlin.h>

#include "Game.hpp"

#include "Camera.hpp"
#include "Block.hpp"
#include "Debug.hpp"
#include "GameState.hpp"
#include "HUD.hpp"
#include "Player.hpp"


void GenerateTerrain();

void StartGame()
{
    player = std::unique_ptr<Player>(new Player(glm::vec3(0, 0, 0)));

    camera = std::unique_ptr<Camera>(new Camera());

    hud = std::unique_ptr<HUD>(new HUD());
    
    GenerateTerrain();
}

void UpdateGame()
{
    player->Update();
}

void RenderGame()
{
    Block::Render();

    hud->Render();
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
    player->ProcessMouseClick(window, button, action, mods);
}

void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    window; xOffset;
    camera->ProcessMouseScroll((float)yOffset);
}

bool AddBlock(BlockIndex blockIndex)
{
    if (blocks[blockIndex] == nullptr)
    {
        glm::vec3 blockPos = glm::vec3(std::get<0>(blockIndex), std::get<1>(blockIndex), std::get<2>(blockIndex));
        blocks[blockIndex] = std::unique_ptr<Block>(new Block(blockPos));
        return true;
    }
    else
    {
        DebugLog("Can't add block, space already has one.");
        return false;
    }
}

bool RemoveBlock(BlockIndex blockIndex)
{
    auto it = blocks.find(blockIndex);
    if (it != blocks.end() && it->second != nullptr)
    {
        blocks.erase(it);
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
    const int terrainThickness = 5;
    const int minX = -60, maxX = 60, minZ = -60, maxZ = 60;
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
                AddBlock(std::make_tuple(x, y, z));
                count++;
            }
        }
    }

    logMsg.str("");
    logMsg << "TERRAIN GEN generated " << count << " blocks.";
    DebugLog(logMsg.str());
}

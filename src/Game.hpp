#pragma once

#include <string>

#include "GameState.hpp"

void StartGame();
void UpdateGame();
void RenderGame();

void ProcessInput(GLFWwindow *window);
void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset);

bool AddBlock(BlockIndex blockIndex);
bool RemoveBlock(BlockIndex blockIndex);

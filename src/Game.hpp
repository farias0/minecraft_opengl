#pragma once

#include <string>

void StartGame();
void UpdateGame();
void RenderGame();

void ProcessInput(GLFWwindow *window);
void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset);

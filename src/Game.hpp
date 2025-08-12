#pragma once

#include <string>

void StartGame();
void UpdateGame();
void RenderGame();

void ProcessInput(GLFWwindow *window);
void MouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset);

void DebugLog(std::string message);

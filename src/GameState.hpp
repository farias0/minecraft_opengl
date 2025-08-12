#pragma once

#include <memory>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern double lastFrame;
extern float deltaTime;

class Camera;
extern std::unique_ptr<Camera> camera;

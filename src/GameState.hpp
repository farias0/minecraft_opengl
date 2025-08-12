#pragma once

#include <memory>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern const float GRAVITY;

extern double lastFrame;
extern float deltaTime;

class Camera;
extern std::unique_ptr<Camera> camera;

class Player;
extern std::unique_ptr<Player> player;

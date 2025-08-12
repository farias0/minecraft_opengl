#include "GameState.hpp"

#include "Camera.hpp"
#include "Player.hpp"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

double lastFrame = 0.0;
float deltaTime = 0.0;

std::unique_ptr<Camera> camera = nullptr;

std::unique_ptr<Player> player = nullptr;
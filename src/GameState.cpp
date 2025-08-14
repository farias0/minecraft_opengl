#include "GameState.hpp"

#include "Camera.hpp"
#include "Cube.hpp"
#include "HUD.hpp"
#include "Player.hpp"

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const float GRAVITY = 9.8f;

double lastFrame = 0.0;
float deltaTime = 0.0;

std::unique_ptr<Camera> camera = nullptr;

std::unique_ptr<HUD> hud = nullptr;

std::unique_ptr<Player> player = nullptr;

std::map<std::tuple<int, int, int>, std::unique_ptr<Cube>> cubes;

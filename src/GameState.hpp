#pragma once

#include <map>
#include <memory>

extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

extern const float GRAVITY;

extern double lastFrame;
extern float deltaTime;

class Camera;
extern std::unique_ptr<Camera> camera;

class HUD;
extern std::unique_ptr<HUD> hud;

class Player;
extern std::unique_ptr<Player> player;

class Block;
typedef std::tuple<int, int, int> BlockIndex;
extern std::map<BlockIndex, std::unique_ptr<Block>> blocks;

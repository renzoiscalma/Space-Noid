#pragma once
#include "Entity.h"
#include "BallEntity.h"
#include "BrickEntity.h"
#include "PaddleEntity.h"

extern std::vector<Entity*> deadEntityList;
extern std::vector<BallEntity*> ballEntityList;
extern std::vector<BrickEntity*> brickEntityList;
extern std::vector<Entity*> entityList;
extern std::vector<Explosion*> explosionList;
extern b2World* world;
extern Sprite* ballSprite;
extern enum GameState { START, PLAYING, GAME_OVER };
extern GameState gameState;
extern bool attachedBall;
extern int lives;
extern int level;
extern int maxLevel;
extern int bricksToKill;
extern int gameScore;
extern std::uniform_real_distribution<float> plusMinus5Degrees;
extern std::mt19937 rng;
extern Sprite* darkPaddle;
extern PaddleEntity* paddleEntity;
extern float cursorX;
extern std::vector<std::vector<BrickEntity*>> levels;
extern std::vector<int> bricksToKillPerLevel;

namespace Helper
{
	void AddToDeadList(Entity* e);
	void CleanDeadList();
	std::vector<BrickEntity*> CreateBricksFromLevel(int level);
	void LoadLevels();
	void ResetGame(int level);
	void ClearAllList();
	void LaunchBall();
	void StartGame();
}	

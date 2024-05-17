#pragma once

#include "Entity.h"
#include "BallEntity.h"
#include "PaddleEntity.h"
#include "BrickEntity.h"
#include "Blit3D.h"
#include "CollisionMask.h"
#include "random"

enum class PowerUpType {EXPLOSIVE, MULTI_BALL};

// externed sprites
extern std::vector<Sprite*> explosiveBallSprites;
extern Sprite* multiBallSprite;
extern Sprite* ballSprite;
extern Blit3D* blit3D;
extern b2World* world;
extern std::vector<BallEntity*> ballEntityList;
extern PaddleEntity* paddleEntity;
extern std::uniform_real_distribution<float> plusMinus5Degrees;
extern std::uniform_real_distribution<float> oneIn360;
extern std::mt19937 rng;
extern float currentBallSpeed;

class PowerUpEntity : public Entity
{
public:
	int frameNumber;
	PowerUpType type;
	float radius;
	float animationTimer;
	float frameSlice;

	PowerUpEntity(int type);
	bool HandlePaddleCollision();
	void Draw();
	bool Update(float seconds);
	Explosion* makeExplosion(float scale);
};

PowerUpEntity* MakePowerUp(int type, float xpos, float ypos);


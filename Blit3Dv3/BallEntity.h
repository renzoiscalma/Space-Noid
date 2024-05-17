#pragma once

#include "Entity.h"
#include "CollisionMask.h"
#include "BrickEntity.h"
#include "Explosion.h"
#include "EdgeEntity.h"
#include "random"

enum class BallType {MAIN_BALL, MULTI_BALL, FIRE_BALL};

extern std::vector<Sprite*> explosiveBallSprites;
extern float currentBallSpeed;
extern std::uniform_real_distribution<float> oneIn360;
extern std::mt19937 rng;

class BallEntity : public Entity
{
public:
	BallType ballType;

	/*specific for fireball animation*/
	float animationTimer = 0;
	float frameSlice = 1.f / 50.f;
	int frameNumber = 0;

	BallEntity() 
	{
		typeID = ENTITYBALL;
	}

	void Draw();
	bool Update(float seconds);
	bool HandleBrickCollision(BrickEntity* brick);
	Explosion* makeExplosion(float scale);
	Explosion* makeBallBounceExplosion(b2Vec2 pos, EdgeType edgeType, float scale);
};

BallEntity * MakeBall(Sprite *sprite, float scale, BallType ballType);
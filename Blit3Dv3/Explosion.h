#pragma once

#include "Entity.h"

enum class ExplosionType { FIREBALL, BALL_DEATH, BALL_BOUNCE, BRICK_COLLIDE };

extern std::vector<Sprite*> explosionFireSprites;
extern std::vector<Sprite*> electricBurstSprites;
extern std::vector<Sprite*> ballBounceSprites;
extern std::vector<Sprite*> brickCollideSprites;
class Explosion
{
public:
	Sprite* sprite;
	ExplosionType type;
	int currentFrame;
	int maxFrame;
	b2Vec2 position;				// position in pixels
	float animationTimer;
	float frameSlice;
	float scale;
	float angle;

	bool Update(float seconds);
	void Draw();
};

Explosion* MakeExplosion(ExplosionType type, b2Vec2 position, float scale, float angle = 90.f);
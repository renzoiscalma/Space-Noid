#include "Explosion.h"

Explosion* MakeExplosion(ExplosionType type, b2Vec2 position, float scale, float angle)
{
	Explosion* explosion = new Explosion();

	explosion->type = type;
	explosion->currentFrame = 0;
	explosion->position = position;
	explosion->animationTimer = 0;
	explosion->scale = scale;
	explosion->angle = angle;

	switch (type) {
	case ExplosionType::FIREBALL:
		explosion->maxFrame = 70;
		explosion->sprite = explosionFireSprites[0];
		explosion->frameSlice = 1.f / 30;
		break;
	case ExplosionType::BALL_DEATH:
		explosion->maxFrame = 16;
		explosion->sprite = electricBurstSprites[0];
		explosion->frameSlice = 1.f / 20.f;
		break;
	case ExplosionType::BALL_BOUNCE:
		explosion->maxFrame = 7;
		explosion->sprite = ballBounceSprites[0];
		explosion->frameSlice = 1.f / 18.f;
		break;
	case ExplosionType::BRICK_COLLIDE:
		explosion->maxFrame = 49;
		explosion->sprite = brickCollideSprites[0];
		explosion->frameSlice = 1.f / 60.f;
		break;
	default:
		break;
	}
	return explosion;
}

bool Explosion::Update(float seconds)
{
	if (currentFrame == maxFrame)
		return true;
	animationTimer += seconds;
	if (animationTimer >= frameSlice) {
		switch (type) {
		case ExplosionType::FIREBALL:
			sprite = explosionFireSprites[currentFrame++];
			break;
		case ExplosionType::BALL_DEATH:
			sprite = electricBurstSprites[currentFrame++];
			break;
		case ExplosionType::BALL_BOUNCE:
			sprite = ballBounceSprites[currentFrame++];
			break;
		case ExplosionType::BRICK_COLLIDE:
			sprite = brickCollideSprites[currentFrame++];
			break;
		}
		animationTimer -= frameSlice;
	}
	return false;
}

void Explosion::Draw()
{
	sprite->angle = angle;
	sprite->Blit(position.x, position.y, scale, scale);
	
}
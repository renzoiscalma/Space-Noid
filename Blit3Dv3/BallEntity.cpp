#include "BallEntity.h"
#include "Blit3D.h"

extern Blit3D *blit3D;
extern b2World *world;

// todo refactor for multiple balls
BallEntity * MakeBall(Sprite *sprite, float scale, BallType type)
{
	//make a ball entity
	BallEntity *ballEntity = new BallEntity();

	ballEntity->scale = scale;

	// Define the Ball body. 
	//We set its position and call the body factory.
	b2BodyDef BallBodyDef;
	BallBodyDef.type = b2_dynamicBody; //make it a dynamic body i.e. one moved by physics
	BallBodyDef.position.Set(0.0f, 0.0f); //set its position in the world

	//friction won't work on a rolling circle in Box2D, so apply angular damping to the body
	//to make it slow down as it rolls
	BallBodyDef.angularDamping = 1.8f;


	//make the userdata point back to this entity
	BallBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(ballEntity);

	ballEntity->body = world->CreateBody(&BallBodyDef); //create the body and add it to the world

	// Define a ball shape for our body.
	//A circle shape for our ball
	b2CircleShape dynamicBall;
	dynamicBall.m_radius = (9.f / (2 * PTM_RATIO)) * scale; //ball has diameter of 9 pixels

	//create the fixture definition - we don't need to save this
	b2FixtureDef fixtureDef;

	// Define the dynamic body fixture.
	fixtureDef.shape = &dynamicBall;

	// Set the ball density to be non-zero, so it will be dynamic.
	//Remember, density * area determines the mass of the object
	fixtureDef.density = 1.0f;

	// Override the default friction.
	fixtureDef.friction = 0.0f;

	//restitution makes us bounce; use 0 for no bounce, 1 for perfect bounce
	fixtureDef.restitution = 1.0f;

	fixtureDef.filter.categoryBits = CMASK_BALL;
	fixtureDef.filter.maskBits = CMASK_BALL | CMASK_PADDLE |
		CMASK_EDGES | CMASK_BRICK | CMASK_GROUND;

	// Add the shape to the body. 
	ballEntity->body->CreateFixture(&fixtureDef);

	//add a sprite to the ball entity
	ballEntity->sprite = sprite;

	ballEntity->ballType = type;
	return ballEntity;
}

bool BallEntity::Update(float seconds) {
	b2Vec2 pos = this->body->GetPosition();
	b2Vec2 ballDir = this->body->GetLinearVelocity();

	// sanity check the ball if it's out of bounds
	if (pos.x < 0 || pos.y < 0
		|| pos.x * PTM_RATIO > blit3D->screenWidth
		|| pos.y * PTM_RATIO > blit3D->screenHeight)
	{
		return true;
	}

	animationTimer += seconds;
	if (animationTimer >= frameSlice) {
		switch (ballType) {
		case BallType::FIRE_BALL:
			frameNumber = (frameNumber + 1) % 60;
			sprite = explosiveBallSprites[frameNumber];
			break;
		}
		animationTimer -= frameSlice;
	}

	// make sure speed is constant
	
	float minBallSpeedY = 1.5f * PTM_RATIO;

	if (ballDir.y < 0)
	{
		ballDir.y = ballDir.y > -minBallSpeedY ? -minBallSpeedY : ballDir.y;
	}
	else
	{
		ballDir.y = ballDir.y < minBallSpeedY ? minBallSpeedY : ballDir.y;
	}

	ballDir.Normalize();
	ballDir *= currentBallSpeed;
	this->body->SetLinearVelocity(ballDir);

	return false;
}

bool BallEntity::HandleBrickCollision(BrickEntity* brick)
{
	switch (ballType) {
	case BallType::FIRE_BALL:	// check if within explosion
		float explosionRadius = 200.f;
		b2Vec2 brickPos = Physics2Pixels(brick->body->GetPosition());
		float dx = (this->position.x - brickPos.x);
		float dy = (this->position.y - brickPos.y);
		float centerPointDistance = (dx * dx) + (dy * dy);
		if (explosionRadius * explosionRadius > centerPointDistance)
			return true;
		break;
	}

	return false;
}

void BallEntity::Draw()
{
	position = body->GetPosition();
	position = Physics2Pixels(position);
	b2Vec2 dir = body->GetLinearVelocity();

	switch (ballType)
	{
	case BallType::FIRE_BALL:
		sprite->angle = rad2deg(atan2f(dir.x, -dir.y));
		break;
	default:
		sprite->angle = rad2deg(body->GetAngle());
		break;
	}

	sprite->Blit(position.x, position.y, scale, scale);

}

Explosion* BallEntity::makeExplosion(float scale) {
	Explosion* explosion;
	b2Vec2 position = Physics2Pixels(body->GetPosition());
	switch (ballType)
	{
	case BallType::FIRE_BALL:
		explosion = MakeExplosion(ExplosionType::FIREBALL,
			position, scale, oneIn360(rng));
		break;
	case BallType::MAIN_BALL:
	case BallType::MULTI_BALL:
		explosion = MakeExplosion(ExplosionType::BALL_DEATH,
			position, scale, oneIn360(rng));
		break;
	default:
		explosion = MakeExplosion(ExplosionType::BALL_DEATH,
			position, scale, oneIn360(rng));
		break;
	}
	return explosion;
}

Explosion* BallEntity::makeBallBounceExplosion(b2Vec2 pos, EdgeType edgeType, float scale) {
	Explosion* explosion = NULL;
	// for some reason bounces are not at the ball
	float pos_adjustment = 15.f;
	switch (edgeType) {
	case EdgeType::UP:
		pos.y -= pos_adjustment;
		explosion = MakeExplosion(ExplosionType::BALL_BOUNCE, pos, scale, 180.f);
		break;
	case EdgeType::LEFT:
		pos.x += pos_adjustment;
		explosion = MakeExplosion(ExplosionType::BALL_BOUNCE, pos, scale, 270.f);
		break;
	case EdgeType::RIGHT:
		pos.x -= pos_adjustment;
		explosion = MakeExplosion(ExplosionType::BALL_BOUNCE, pos, scale, 90.f);
		break;
	default:
		pos.y += pos_adjustment;
		explosion = MakeExplosion(ExplosionType::BALL_BOUNCE, pos, scale);
		break;
	}
	return explosion;
}
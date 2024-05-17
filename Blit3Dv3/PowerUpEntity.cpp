#include "PowerUpEntity.h"

PowerUpEntity::PowerUpEntity(int type)
{
	typeID = ENTITYPOWERUP;
	frameNumber = 0;
	scale = 3;
	animationTimer = 0;
	frameSlice = 1.f / 20.f;

	switch (type) {
	case 1:
		this->type = PowerUpType::EXPLOSIVE;
		this->radius = 9.f / (2 * PTM_RATIO);
		this->sprite = explosiveBallSprites[0];
		break;
	case 2:
		this->type = PowerUpType::MULTI_BALL;
		this->radius = 8.f / (2 * PTM_RATIO);
		this->sprite = multiBallSprite;
		break;
	default:
		this->type = PowerUpType::MULTI_BALL;
		this->radius = 8.f / (2 * PTM_RATIO);
		this->sprite = multiBallSprite;
	}
}

PowerUpEntity* MakePowerUp(int type, float xpos, float ypos) 
{
	PowerUpEntity* powerUpEntity = new PowerUpEntity(type);

	b2BodyDef powerUpBodyDef;
	powerUpBodyDef.type = b2_dynamicBody;
	powerUpBodyDef.position.Set(xpos / PTM_RATIO, ypos / PTM_RATIO);
	powerUpBodyDef.angularDamping = 1.8f;

	powerUpBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(powerUpEntity);
	powerUpEntity->body = world->CreateBody(&powerUpBodyDef);

	b2CircleShape powerUpShape;
	powerUpShape.m_radius = powerUpEntity->radius * powerUpEntity->scale;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &powerUpShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 1.0;

	fixtureDef.filter.categoryBits = CMASK_POWERUP;
	fixtureDef.filter.maskBits = CMASK_PADDLE | CMASK_GROUND;

	powerUpEntity->body->CreateFixture(&fixtureDef);

	b2Vec2 dir = (50.f + plusMinus5Degrees(rng)) * deg2vec(270);
	powerUpEntity->body->SetLinearVelocity(b2Vec2(0, 0));
	powerUpEntity->body->ApplyLinearImpulse(dir, powerUpEntity->body->GetPosition(), true);

	return powerUpEntity;
}

bool PowerUpEntity::HandlePaddleCollision() 
{
	BallEntity *ball1, *ball2, *fireball;
	b2Vec2 ballSpawnPos, dir1, dir2, dir;

	switch (type) {
	case PowerUpType::MULTI_BALL:
		dir1 = currentBallSpeed * deg2vec(45 + plusMinus5Degrees(rng));
		dir2 = currentBallSpeed * deg2vec(135 + plusMinus5Degrees(rng));

		ballSpawnPos = this->body->GetPosition();
		ball1 = MakeBall(ballSprite, 3, BallType::MULTI_BALL);
		ball1->body->SetTransform(ballSpawnPos, 0);
		ball1->body->SetLinearVelocity(dir1);
		ball2 = MakeBall(ballSprite, 3, BallType::MULTI_BALL);
		ball2->body->SetTransform(ballSpawnPos, 0);
		ball2->body->SetLinearVelocity(dir2);

		ballEntityList.push_back(ball1);
		ballEntityList.push_back(ball2);

		return true;
		break;
	case PowerUpType::EXPLOSIVE:
		dir = currentBallSpeed * deg2vec(75 + plusMinus5Degrees(rng));
		ballSpawnPos = this->body->GetPosition();
		fireball = MakeBall(explosiveBallSprites[0], 3, BallType::FIRE_BALL);
		fireball->body->SetTransform(ballSpawnPos, 0);
		fireball->body->SetLinearVelocity(dir);
		ballEntityList.push_back(fireball);
		return true;
		break;
	}
}

Explosion* PowerUpEntity::makeExplosion(float scale)
{
	Explosion* explosion = NULL;
	b2Vec2 position = Physics2Pixels(body->GetPosition());
	switch (type) {
	case PowerUpType::EXPLOSIVE:
		explosion = MakeExplosion(ExplosionType::FIREBALL,
			position, scale, oneIn360(rng));
		break;
	case PowerUpType::MULTI_BALL:
		explosion = MakeExplosion(ExplosionType::BALL_DEATH,
			position, scale, oneIn360(rng));
		break;
	}
	return explosion;
}

void PowerUpEntity::Draw() 
{
	position = body->GetPosition();
	position = Physics2Pixels(position);
	b2Vec2 dir = body->GetLinearVelocity();
	sprite->angle = rad2deg(atan2f(dir.x, -dir.y));

	// Draw the objects
	sprite->Blit(position.x, position.y, scale, scale);
}

bool PowerUpEntity::Update(float seconds) {
	animationTimer += seconds;
	if (animationTimer >= frameSlice) {
		switch (type) {
		case PowerUpType::EXPLOSIVE:
				frameNumber = (frameNumber + 1) % 60;
				sprite = explosiveBallSprites[frameNumber];
			break;
		}
		animationTimer -= frameSlice;
	}
	return false;
}
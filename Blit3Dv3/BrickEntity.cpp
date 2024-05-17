#include "BrickEntity.h"

//Decide whether to change colour or die off:
//return true if this pbject should be removed
bool BrickEntity::HandleCollision()
{
	if (colour == BrickColour::GREY)
		return false;

	int newColour = (static_cast<int>(colour) - 1);

	if (newColour <= 0)
		return true;

	colour = static_cast<BrickColour>(newColour);
	sprite = brickSpriteList[static_cast<int>(type)][newColour];

	return false;
}

extern b2World *world;

BrickEntity* MakeBrick(BrickColour colour, BrickType type, float xpos, float ypos, float scale)
{
	BrickEntity *brickEntity = new BrickEntity();
	brickEntity->colour = colour;
	brickEntity->type = type;
	brickEntity->scale = scale;
	brickEntity->position = b2Vec2(xpos, ypos);
	//set the sprite to draw with
	brickEntity->sprite = brickSpriteList[static_cast<int>(type)][static_cast<int>(colour)];

	switch (type)
	{
	case BrickType::RECT:
		brickEntity->halfWidth = (RECT_BRICK_WIDTH / (2.f * PTM_RATIO)) ;
		brickEntity->halfHeight = (RECT_BRICK_HEIGHT / (2.f * PTM_RATIO));
		break;
	case BrickType::SQUARE:
		brickEntity->halfWidth = (SQUARE_BRICK_WIDTH / (2.f * PTM_RATIO));
		brickEntity->halfHeight = (SQUARE_BRICK_HEIGHT / (2.f * PTM_RATIO));
		break;
	}
	return brickEntity;
}

void BrickEntity::AddToWorld()
{

	//make the physics body
	b2BodyDef brickBodyDef;

	//set the position of the center of the body, 
	//converting from pxel coords to physics measurements
	brickBodyDef.position.Set(position.x / PTM_RATIO, position.y / PTM_RATIO);
	brickBodyDef.type = b2_kinematicBody; //make it a kinematic body i.e. one moved by us

	//make the userdata point back to this entity
	brickBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(this);

	body = world->CreateBody(&brickBodyDef); //create the body and add it to the world

	// Define a box shape for our dynamic body.
	b2PolygonShape boxShape;
	//SetAsBox() takes as arguments the half-width and half-height of the box
	boxShape.SetAsBox(halfWidth * scale, halfHeight * scale);

	b2FixtureDef brickFixtureDef;
	brickFixtureDef.shape = &boxShape;
	brickFixtureDef.density = 1.0f; //won't matter, as we made this kinematic
	brickFixtureDef.restitution = 0;
	brickFixtureDef.friction = 0.1f;
	brickFixtureDef.filter.categoryBits = CMASK_BRICK;
	brickFixtureDef.filter.maskBits = CMASK_BALL;

	body->CreateFixture(&brickFixtureDef);
}
#include "Entity.h"

Entity::Entity()
{
	sprite = NULL;
	body = NULL;
	typeID = ENTITYNONE;	// default entity type
	scale = 1;				// scaling for bigger screens
}

Entity::~Entity()
{
	
};

void Entity::Draw()
{
	position = body->GetPosition();
	position = Physics2Pixels(position);
	sprite->angle = rad2deg(body->GetAngle());

	// Draw the objects
	sprite->Blit(position.x, position.y, scale, scale);
}

bool Entity::Update(float seconds)
{
	return false;
}
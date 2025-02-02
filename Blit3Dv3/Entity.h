#pragma once

#include "Blit3D.h"

#include "Physics.h"

enum EntityTypes { ENTITYNONE = 0, ENTITYBALL, ENTITYBRICK, ENTITYPADDLE, ENTITYEDGE, ENTITYPOWERUP, EXPLOSION };

//a simple entity class that connects the physics object and the render object
class Entity
{
public:
	Sprite *sprite;
	b2Body *body;
	b2Vec2 position; //used to calculate the position of the object
	EntityTypes typeID;
	float scale;

	Entity();
	virtual ~Entity(); //always make base class destructor virtual
	virtual void Draw();
	virtual bool Update(float seconds);
};
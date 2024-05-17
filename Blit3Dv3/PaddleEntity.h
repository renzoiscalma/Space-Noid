#pragma once

#include "Entity.h"
#include "Blit3D.h"
#include "CollisionMask.h"
class PaddleEntity : public Entity
{
public:
	float halfWidth;
	float bend;
	float halfHeight;

	PaddleEntity()
	{
		typeID = ENTITYPADDLE;
		halfWidth = 32 / (2 * PTM_RATIO); //defualt paddle is 32 pixels wide
		halfHeight = 7 / (2 * PTM_RATIO);
		bend = 0.8f; //increasing this increases the amount of bending applied to the ball
	}

	//Method to "bend" flight of ball that hits the paddle
	void HandleCollision(b2Body *ballBody);
};

PaddleEntity * MakePaddle(float xpos, float ypos, Sprite *sprite, float scale);
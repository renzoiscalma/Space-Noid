#pragma once

#include "Entity.h"
#include "CollisionMask.h"

enum class BrickColour {GREY = 0, GREEN, YELLOW, ORANGE, RED, PURPLE};
enum class BrickType {RECT = 0, SQUARE};

#define SQUARE_BRICK_WIDTH 16
#define SQUARE_BRICK_HEIGHT 16
#define RECT_BRICK_WIDTH 32
#define RECT_BRICK_HEIGHT 16
//externed sprites
extern std::vector<Sprite*> squareSpriteList;
extern std::vector<Sprite*> rectSpriteList;
extern std::vector<std::vector<Sprite*>> brickSpriteList; // 0 for rect sprite list, 1 for square

class BrickEntity : public Entity
{
public:
	BrickColour colour;
	BrickType type;
	float halfWidth;
	float halfHeight;
	BrickEntity()
	{
		typeID = ENTITYBRICK;
	}

	bool HandleCollision();
	void AddToWorld();
};

BrickEntity* MakeBrick(BrickColour color, BrickType type, float xpos, float ypos, float scale = 3.f);
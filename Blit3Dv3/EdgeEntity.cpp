#include "EdgeEntity.h"
#include "CollisionMask.h"

EdgeEntity* MakeEdge(EdgeType edgeType)
{
	EdgeEntity* edgeEntity = new EdgeEntity(edgeType);
	//A bodyDef for the ground
	b2BodyDef edgeBodyDef;

	// Define the edge body.
	edgeBodyDef.position.Set(0, 0);
	//add the userdata
	edgeBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(edgeEntity);

	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	edgeEntity->body = world->CreateBody(&edgeBodyDef);

	b2EdgeShape edgeShape;

	b2FixtureDef edgeFixtureDef;
	edgeFixtureDef.shape = &edgeShape;

	switch (edgeType) {
	case EdgeType::DOWN:
		//collison masking
		edgeFixtureDef.filter.categoryBits = CMASK_GROUND;					//this is the ground
		edgeFixtureDef.filter.maskBits = CMASK_BALL | CMASK_POWERUP;		//it collides wth balls and powerups
		edgeShape.SetTwoSided(b2Vec2(0, 0), b2Vec2(blit3D->screenWidth / PTM_RATIO, 0));
		break;
	case EdgeType::UP:
		edgeFixtureDef.filter.categoryBits = CMASK_EDGES;
		edgeFixtureDef.filter.maskBits = CMASK_BALL;		//it collides wth balls
		edgeShape.SetTwoSided(b2Vec2(0, blit3D->screenHeight / PTM_RATIO),
			b2Vec2(blit3D->screenWidth / PTM_RATIO, blit3D->screenHeight / PTM_RATIO));
		break;
	case EdgeType::LEFT:
		edgeShape.SetTwoSided(b2Vec2(0, blit3D->screenHeight / PTM_RATIO), b2Vec2(0, 0));
		edgeFixtureDef.filter.categoryBits = CMASK_EDGES;
		edgeFixtureDef.filter.maskBits = CMASK_BALL;		//it collides wth balls
		break;
	case EdgeType::RIGHT:
		edgeShape.SetTwoSided(b2Vec2(blit3D->screenWidth / PTM_RATIO,
			0), b2Vec2(blit3D->screenWidth / PTM_RATIO, blit3D->screenHeight / PTM_RATIO));
		edgeFixtureDef.filter.categoryBits = CMASK_EDGES;
		edgeFixtureDef.filter.maskBits = CMASK_BALL;		//it collides wth balls
		break;
	default:
		break;
	}
	edgeEntity->body->CreateFixture(&edgeFixtureDef);
	return edgeEntity;
}
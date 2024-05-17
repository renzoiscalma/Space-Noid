#pragma once

#include "Entity.h"

extern b2World* world;
extern Blit3D* blit3D;

enum class EdgeType {UP, DOWN, LEFT, RIGHT};

class EdgeEntity : public Entity
{
public:
	EdgeType edge;
	EdgeEntity(EdgeType edge)
	{
		typeID = ENTITYEDGE;
		this->edge = edge;
	}

	void Draw() {}; //empty draw, as ground does not draw
};

EdgeEntity* MakeEdge(EdgeType edge);

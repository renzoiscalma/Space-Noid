#include "Helper.h"

void Helper::AddToDeadList(Entity* e) 
{
	bool unique = true;
	for (auto& entity : deadEntityList)
	{
		if (entity == e) {
			unique = false;
			break;
		}
	}

	if (unique) deadEntityList.push_back(e);
}

void Helper::CleanDeadList()
{
	//clean up dead entities
	for (auto& e : deadEntityList)
	{
		//std::cout << "deleting " << e->typeID << reinterpret_cast<void*>(e) << std::endl;
		//remove body from the physics world and free the body data
		world->DestroyBody(e->body);
		//remove the entity from the appropriate entityList
		if (e->typeID == ENTITYBALL)
		{
			for (int i = 0; i < ballEntityList.size(); ++i)
			{
				if (e == ballEntityList[i])
				{
					delete ballEntityList[i];
					ballEntityList.erase(ballEntityList.begin() + i);
					break;
				}
			}
		}
		if (e->typeID == ENTITYBRICK)
		{
			for (int i = 0; i < brickEntityList.size(); ++i)
			{
				if (e == brickEntityList[i])
				{
					delete brickEntityList[i];
					brickEntityList.erase(brickEntityList.begin() + i);
				}
			}
		}
		else
		{
			for (int i = 0; i < entityList.size(); ++i)
			{
				if (e == entityList[i])
				{
					delete entityList[i];
					entityList.erase(entityList.begin() + i);
					break;
				}
			}
		}
	}

	deadEntityList.clear();
}

// loads a brick level and returns as a new vector of BrickEntity*
// also adds the brick to the world
std::vector<BrickEntity*> Helper::CreateBricksFromLevel(int level)
{
	std::vector<BrickEntity*> result;
	for (int i = 0; i < levels[level].size(); i++) {
		BrickEntity* currBrick = levels[level][i];
		BrickEntity* dupeBrick = MakeBrick(currBrick->colour, currBrick->type, currBrick->position.x, currBrick->position.y);
		dupeBrick->AddToWorld();
		result.push_back(dupeBrick);
	}
	return result;
}

// empties all list, then creates a level based on the integer passed.
void Helper::ResetGame(int level)
{
 	ClearAllList();
	
	//______MAKE A BALL_______________________
	BallEntity *ball = MakeBall(ballSprite, 3, BallType::MAIN_BALL);

	b2Vec2 ballPos = paddleEntity->body->GetPosition();
	ballPos.y = 65 / PTM_RATIO;
	ball->body->SetTransform(ballPos, 0.f);
	ball->body->SetLinearVelocity(b2Vec2(0.f, 0.f));
	ballEntityList.push_back(ball);

	//______MAKE SOME BRICKS_______________________
	brickEntityList = CreateBricksFromLevel(level);
	bricksToKill = bricksToKillPerLevel[level];
	
}

void Helper::ClearAllList()
{
	for (auto& e : entityList) {
		world->DestroyBody(e->body);
	}
	entityList.clear();

	for (auto& b : ballEntityList) {
		world->DestroyBody(b->body);
	}
	ballEntityList.clear();

	for (auto& d : deadEntityList) {
		world->DestroyBody(d->body);
	}
	deadEntityList.clear();

	for (auto& b : brickEntityList) {
		world->DestroyBody(b->body);
	}
	brickEntityList.clear();

	explosionList.clear();
}

void Helper::StartGame()
{
	gameState = PLAYING;
	attachedBall = true;
	lives = 3;
}

void Helper::LaunchBall()
{
	if (attachedBall)
	{
		attachedBall = false;
		Entity* b = ballEntityList[0];
		//kick the ball in a random direction upwards			
		b2Vec2 dir = deg2vec(90 + plusMinus5Degrees(rng)); //between 85-95 degrees
		//make the ball move
		dir *= currentBallSpeed; //scale up the velocity
		b->body->SetLinearVelocity(dir); //apply velocity to kinematic object				
	}
}

void Helper::LoadLevels() {
	std::string fileName = "levels.dat";
	//open file
	std::ifstream levelsFile;

	levelsFile.open(fileName);

	if (levelsFile.is_open())
	{
		int numLevels = 0;
		levelsFile >> numLevels;
		for (int i = 0; i < numLevels; i++) {
			std::vector<BrickEntity*> brickList;
			//read in # of bricks
			int brickNum = 0;
			levelsFile >> brickNum;
			//read in each brick
			int currBricksToKill = 0;
			for (; brickNum > 0; --brickNum)
			{
				//make a brick
				int type, color;
				float x, y;
				levelsFile >> type;
				levelsFile >> color;
				levelsFile >> x;
				levelsFile >> y;
				BrickEntity* B = MakeBrick((BrickColour)color, (BrickType)type, x, y);
				brickList.push_back(B);
				if (B->colour != BrickColour::GREY)
					currBricksToKill++;
			}

			// push to list of levels
			levels.push_back(brickList);
			bricksToKillPerLevel.push_back(currBricksToKill);
		}
		// set globla variable maxLevel
		maxLevel = numLevels;

		levelsFile.close();
	}
}
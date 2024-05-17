/*
Example program that demonstrates collision handling
*/
#include "Blit3D.h"

#include <random>

#include "Physics.h"
#include "Entity.h"
#include "PaddleEntity.h"
#include "BallEntity.h"
#include "BrickEntity.h"
#include "EdgeEntity.h"
#include "Explosion.h"
#include "PowerUpEntity.h"
#include "MyContactListener.h"
#include "Camera.h"
#include "Helper.h"
Blit3D *blit3D = NULL;

//this code sets up memory leak detection
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


//GLOBAL DATA
std::mt19937 rng;
std::uniform_real_distribution<float> plusMinus5Degrees(-5, +5);
std::uniform_real_distribution<float> oneInFour(1, 5);
std::uniform_real_distribution<float> oneInThree(1, 4);
std::uniform_real_distribution<float> oneInTwo(1, 3);
std::uniform_real_distribution<float> oneIn360(1, 361);
b2Vec2 gravity; //defines our gravity vector
b2World *world; //our physics engine

				// Prepare for simulation. Typically we use a time step of 1/60 of a
				// second (60Hz) and ~10 iterations. This provides a high quality simulation
				// in most game scenarios.
int32 velocityIterations = 8;
int32 positionIterations = 3;
float timeStep = 1.f / 60.f; //one 60th of a second
float elapsedTime = 0; //used for calculating time passed

std::vector<Entity *> entityList; //most of the entities in our game go here
std::vector<BallEntity *> ballEntityList; //track the balls seperately from everything else
std::vector<BrickEntity *> brickEntityList;
std::vector<Entity *> deadEntityList;
std::vector<Explosion *> explosionList;
std::vector<std::vector<BrickEntity*>> levels;
std::vector<int> bricksToKillPerLevel;				// bricks to kill per level

EdgeEntity* edgeUp = NULL;
EdgeEntity* edgeDown = NULL;
EdgeEntity* edgeLeft = NULL;
EdgeEntity* edgeRight = NULL;

//contact listener to handle collisions between important objects
MyContactListener *contactListener;

PaddleEntity *paddleEntity = NULL;

GameState gameState;
int lives = 3;
int gameScore = 0;
int level = 0;
int maxLevel = 0;
int bricksToKill;
bool attachedBall = true; //is the ball ready to be launched from the paddle?
bool startVisible = false;
bool gameOverVisible = false;
bool pausedForNextRound = true;
float currentBallSpeed = 55; //defaultball speed
float cursorX = 0;
// timers for text
float flickerTimer = 0;
float nextLevelTimer = 2.f;
float goTimer = 0.f;

//Sprites 
Sprite *logo = NULL;
Sprite *ballSprite = NULL;
Sprite *redBrickSprite = NULL;
Sprite *yellowBrickSprite = NULL;
Sprite *orangeBrickSprite = NULL;
Sprite *paddleSprite = NULL;
Sprite* darkPaddle = NULL;
Sprite* multiBallSprite = NULL;
Sprite* background = NULL; 

std::vector<Sprite*> explosionFireSprites;
std::vector<Sprite*> explosiveBallSprites;
std::vector<Sprite*> ballBounceSprites;
std::vector<Sprite*> electricBurstSprites;
std::vector<Sprite*> brickCollideSprites;
std::vector<Sprite*> squareSpriteList;
std::vector<Sprite*> rectSpriteList;
std::vector<std::vector<Sprite*>> brickSpriteList; // 0 for rect sprite list, 1 for square

Camera2D* camera = NULL; //for screenshake

//font
AngelcodeFont *debugFont = NULL;

void Init()
{
	//seed random generator
	std::random_device rd;
	rng.seed(rd());

	//turn off the cursor
	blit3D->ShowCursor(false);

	debugFont = blit3D->MakeAngelcodeFontFromBinary32("Media\\TheBoldFont.bin");

	background = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\bg.png");
	for (int i = 0; i < 7; i++) {
		rectSpriteList.push_back(blit3D->MakeSprite(112, (i * 16), 32, 16, "Media\\bricks.png"));
	}
	brickSpriteList.push_back(rectSpriteList);
	for (int i = 0; i < 7; i++) {
		squareSpriteList.push_back(blit3D->MakeSprite(224, (i * 16), 16, 16, "Media\\bricks.png"));
	}
	brickSpriteList.push_back(squareSpriteList);

	logo = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\splash.png");
	ballSprite = blit3D->MakeSprite(160, 37, 11, 11, "Media\\paddles_and_balls.png");
	multiBallSprite = blit3D->MakeSprite(144, 40, 8, 8, "Media\\paddles_and_balls.png");
	paddleSprite = blit3D->MakeSprite(236, 175, 42, 11, "Media\\arinoid_master.png");

	darkPaddle = blit3D->MakeSprite(0, 7, 32, 9, "Media\\paddles_and_balls.png");


	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 10; j++)
			explosiveBallSprites.push_back(blit3D->MakeSprite((j * 10), (i * 26), 10, 26, "Media\\fireball.png"));
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 7; j++) {
			explosionFireSprites.push_back(blit3D->MakeSprite((j * 100), (i * 100), 100, 100, "Media\\explosion_fire.png"));
		}
	}

	for (int i = 0; i < 7; i++) {
		ballBounceSprites.push_back(blit3D->MakeSprite((i * 140), 0, 140, 50, "Media\\ball_bounce.png"));
	}

	for (int i = 0; i < 16; i++) {
		electricBurstSprites.push_back(blit3D->MakeSprite((i * 72), 0, 72, 72, "Media\\electric_burst.png"));
	}

	for (int i = 0; i < 49; i++) {
		brickCollideSprites.push_back(blit3D->MakeSprite((i * 100), 0, 100, 100, "Media\\brick_collide.png"));
	}

	gameState = START;

	//setup camera
	camera = new Camera2D();

	//set it's valid pan area
	camera->minX = -blit3D->screenWidth / 2;
	camera->minY = -blit3D->screenHeight / 2;
	camera->maxX = blit3D->screenWidth / 2;
	camera->maxY = blit3D->screenHeight / 2;

	camera->PanTo(blit3D->screenWidth / 2, blit3D->screenHeight / 2);

	//from here on, we are setting up the Box2D physics world model

	// Define the gravity vector.
	gravity.x = 0.f;
	gravity.y = 0.f;

	// Construct a world object, which will hold and simulate the rigid bodies.
	world = new b2World(gravity);
	//world->SetGravity(gravity); <-can call this to change gravity at any time
	world->SetAllowSleeping(true); //set true to allow the physics engine to 'sleep" objects that stop moving

	//make an entity for the ground
	edgeUp = MakeEdge(EdgeType::UP);
	edgeDown = MakeEdge(EdgeType::DOWN);
	edgeLeft = MakeEdge(EdgeType::LEFT);
	edgeRight = MakeEdge(EdgeType::RIGHT);

	//paddle
	cursorX = blit3D->screenWidth / 2;
	paddleEntity = MakePaddle(blit3D->screenWidth / 2, 80, darkPaddle, 5);

	// Create contact listener and use it to collect info about collisions
	contactListener = new MyContactListener();
	world->SetContactListener(contactListener);

	Helper::LoadLevels();
}

void DeInit(void)
{
	//delete all the entities
	for (auto &e : entityList) delete e;
	entityList.clear();

	for (auto &b : ballEntityList) delete b;
	ballEntityList.clear();

	for (auto& d : deadEntityList) delete d;
	deadEntityList.clear();

	for (auto& b : brickEntityList) delete b;
	brickEntityList.clear();

	for (auto& e : explosionList) delete e;
	explosionList.clear();

	if (camera != NULL) delete camera;

	if (edgeDown != NULL) delete edgeDown;

	if (edgeUp != NULL) delete edgeUp;

	if (edgeLeft != NULL) delete edgeLeft;

	if (edgeRight != NULL) delete edgeRight;

	if (paddleEntity != NULL) delete paddleEntity;

	//delete the contact listener
	delete contactListener;

	//Free all physics game data we allocated
	delete world;

	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	//stop it from lagging hard if more than a small amount of time has passed
	if (seconds > 1.0 / 30) elapsedTime += 1.f / 30;
	else elapsedTime += (float)seconds;

	//don't apply physics unless at least a timestep worth of time has passed
	while (elapsedTime >= timeStep)
	{
		//update the physics world
		world->Step(timeStep, velocityIterations, positionIterations);

		// Clear applied body forces. 
		world->ClearForces();

		// update camera
		camera->Update(timeStep);

		elapsedTime -= timeStep;

		if (nextLevelTimer > 0)
			nextLevelTimer -= timeStep;

		if (goTimer > 0)
			goTimer -= timeStep;

		switch (gameState) {
		case START:
			flickerTimer -= timeStep;
			if (flickerTimer <= 0) {
				startVisible = !startVisible;
				flickerTimer = 1;
			}
			break;
		case GAME_OVER:
			flickerTimer -= timeStep;
			if (flickerTimer <= 0) {
				gameOverVisible = !gameOverVisible;
				flickerTimer = 1;
			}
			break;
		case PLAYING:
			//move paddle to where the cursor is
			b2Vec2 paddlePos;
			paddlePos.y = 30;

			float halfWidthPixels = paddleEntity->halfWidth * PTM_RATIO * paddleEntity->scale;
			if (cursorX < halfWidthPixels) paddlePos.x = halfWidthPixels;
			else if (cursorX > blit3D->screenWidth - halfWidthPixels) paddlePos.x = blit3D->screenWidth - halfWidthPixels;
			else paddlePos.x = cursorX;

			paddlePos = Pixels2Physics(paddlePos);
			paddleEntity->body->SetTransform(paddlePos, 0);

			//sanity check, should always have at least one ball!
			if (ballEntityList.size() == 0)
			{
				BallEntity* ball = MakeBall(ballSprite, 3, BallType::MAIN_BALL);
				ballEntityList.push_back(ball);
				attachedBall = true;
			}

			//if ball is attached to paddle, move ball to where paddle is
			if (attachedBall)
			{
				//assert(ballEntityList.size() == 1); //make sure there is exactly one ball
				b2Vec2 ballPos = paddleEntity->body->GetPosition();
				ballPos.y = 65 / PTM_RATIO;
				ballEntityList[0]->body->SetTransform(ballPos, 0);
				ballEntityList[0]->body->SetLinearVelocity(b2Vec2(0.f, 0.f));
			}

			for (auto& e : entityList) {
				if (e->Update(timeStep))
					Helper::AddToDeadList(e);
			}

			for (auto& b : ballEntityList) {
				if (b->Update(timeStep))
					Helper::AddToDeadList(b);
			}

			for (auto& b : brickEntityList)
			{
				if (b->Update(timeStep))
					Helper::AddToDeadList(b);
			}

			// update explosions
			for (int i = explosionList.size() - 1; i >= 0; i--) {
				if (explosionList[i]->Update(timeStep))
					explosionList.erase(explosionList.begin() + i);
			}

			//loop over contacts

			int numBalls = 0;
			for (auto& ball : ballEntityList) {
				if (ball->ballType == BallType::MAIN_BALL ||
					ball->ballType == BallType::MULTI_BALL)
				{
					numBalls++;
					// do not break since we need to iterate through the 
					// etnire list in order to determine if there is more than 1 ball
				}
			}

			for (int pos = 0; pos < contactListener->contacts.size(); ++pos)
			{
				MyContact contact = contactListener->contacts[pos];

				//fetch the entities from the body userdata
				Entity* A = (Entity*)contact.fixtureA->GetBody()->GetUserData().pointer;
				Entity* B = (Entity*)contact.fixtureB->GetBody()->GetUserData().pointer;
				if (A != NULL && B != NULL) //if there is an entity for these objects...
				{

					if (A->typeID == ENTITYBALL)
					{
						//swap A and B
						Entity* C = A;
						A = B;
						B = C;
					}

					if (B->typeID == ENTITYBALL && A->typeID == ENTITYBRICK)
					{
						BrickEntity* be = (BrickEntity*)A;
						BallEntity* ball = (BallEntity*)B;
						if (ball->ballType == BallType::FIRE_BALL) 
						{
							// make explosion
							explosionList.push_back(ball->makeExplosion(7.f));
							Helper::AddToDeadList(ball);
							// loop each bricks to know if it hits
							for (auto& entity : brickEntityList) 
							{
								BrickEntity* brick;
								if (entity->typeID == ENTITYBRICK) 
								{
									brick = (BrickEntity*)entity;
									if (ball->HandleBrickCollision(brick) 
										&& brick->HandleCollision()) 
									{
										Helper::AddToDeadList(brick);
										bricksToKill--;
										gameScore += 1000;
									}
								}
							}
							camera->Shake(4);
						} 
						else if (be->HandleCollision())
						{
							int powerUpRoll = oneInThree(rng);
							if (powerUpRoll == 1) {
								int powerUpType = oneInTwo(rng);
								PowerUpEntity* powerUp = MakePowerUp(powerUpType, be->position.x, be->position.y);
								entityList.push_back(powerUp);
							}
							//we need to remove this brick from the world, 
							//but can't do that until all collisions have been processed
							Helper::AddToDeadList(be);
							Explosion* explosion = MakeExplosion(ExplosionType::BRICK_COLLIDE,
								Physics2Pixels(contact.contactPoint), 1, oneIn360(rng));
							explosionList.push_back(explosion);
							bricksToKill--;
							gameScore += 1000;
						}
						else {
							Explosion* explosion = MakeExplosion(ExplosionType::BRICK_COLLIDE,
								Physics2Pixels(contact.contactPoint), 1, oneIn360(rng));
							explosionList.push_back(explosion);
						}
						
					}

					if (B->typeID == ENTITYBALL && A->typeID == ENTITYPADDLE)
					{
						PaddleEntity* pe = (PaddleEntity*)A;
						//bend the ball's flight
						pe->HandleCollision(B->body);
					}

					if (B->typeID == ENTITYBALL && A->typeID == ENTITYEDGE)
					{
						BallEntity* ball = (BallEntity*)B;
						EdgeEntity* e = (EdgeEntity*)A;
						if (e->edge == EdgeType::DOWN) {
							if (numBalls > 1 || ball->ballType == BallType::FIRE_BALL)
							{
								Helper::AddToDeadList(ball);
								explosionList.push_back(ball->makeExplosion(1));
								camera->Shake(4);
							}
							else
							{
								//lose a life
								lives--;
								attachedBall = true; //attach the ball for launching
								if (ballEntityList[0]->ballType == BallType::FIRE_BALL) // for the rare chance that index 0 becomes a fireball
								{
									ballEntityList.insert(ballEntityList.begin(), MakeBall(ballSprite, 3, BallType::MAIN_BALL));
								}
								ballEntityList[0]->body->SetLinearVelocity(b2Vec2(0.f, 0.f));
								explosionList.push_back(ball->makeExplosion(1));
								camera->Shake(20);
								if (lives < 1)
								{
									gameState = GAME_OVER;
								}
							}
						}
						else {
							explosionList.push_back(
								ball->makeBallBounceExplosion(Physics2Pixels(contact.contactPoint), 
									e->edge, 0.7f));
							camera->Shake(3);
						}
					}

					//check and see if paddle or ground hit a powerup
					if (A->typeID == ENTITYPOWERUP)
					{
						//swap A and B
						Entity* C = A;
						A = B;
						B = C;
					}

					if (B->typeID == ENTITYPOWERUP) {
						PowerUpEntity* up = (PowerUpEntity*)B;
						switch (A->typeID) {
						case ENTITYPADDLE: 
							if (up->HandlePaddleCollision()) {
								Helper::AddToDeadList(up);
							}
							break;
						case ENTITYEDGE:
							explosionList.push_back(
								up->makeExplosion(1.f)
							);
							Helper::AddToDeadList(up);
							camera->Shake(2);
							break;
						}
					}
				}//end of checking if they are both NULL userdata
			}//end of collison handling

			Helper::CleanDeadList();


			// check if ready for level up
			if (bricksToKill <= 0 && !pausedForNextRound) {
				level++;
				attachedBall = true; //attach the ball for launching
				pausedForNextRound = true;
				// start next level timer in-game semi-pause
				nextLevelTimer = 2.f;
			}

			if (nextLevelTimer <= 0 && pausedForNextRound) {
				pausedForNextRound = false;
				Helper::ResetGame(level % maxLevel);
				goTimer = 2.f;
			}
			break;
		}

	}
}

void Draw(void)
{
	glClearColor(0.8f, 0.6f, 0.7f, 0.0f);	//clear colour: r,g,b,a 	
											// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float startWidthStr, gameOverWidthStr, scoreWidthStr;
	std::string finalScoreStr;
	camera->Draw();

	switch (gameState)
	{
	case START:
		logo->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		startWidthStr = debugFont->WidthText("PRESS SPACE TO START");

		if (startVisible)
			debugFont->BlitText(blit3D->screenWidth / 2 - startWidthStr / 2,
				blit3D->screenHeight / 2 - 200, "PRESS SPACE TO START!");
		break;
	case GAME_OVER:
		background->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		gameOverWidthStr = debugFont->WidthText("GAME OVER PRESS SPACE TO PLAY AGAIN");
		if (gameOverVisible)
			debugFont->BlitText(blit3D->screenWidth / 2 - gameOverWidthStr / 2,
				blit3D->screenHeight / 2, "GAME OVER! PRESS SPACE TO PLAY AGAIN");
		finalScoreStr = "FINAL SCORE: " + std::to_string(gameScore);
		scoreWidthStr = debugFont->WidthText(finalScoreStr);
		debugFont->BlitText(blit3D->screenWidth / 2 - scoreWidthStr / 2,
			blit3D->screenHeight / 2 - 200, finalScoreStr);
		break;
	case PLAYING:
		background->Blit(blit3D->screenWidth / 2, blit3D->screenHeight / 2);
		//loop over all entities and draw them
		for (auto &e : entityList) e->Draw();
		for (auto &b : ballEntityList) b->Draw();
		for (auto& b : brickEntityList) b->Draw();
		for (auto& e : explosionList) e->Draw();
		// draw paddle
		paddleEntity->Draw();

		// show lives
		std::string lifeString = "Lives: " + std::to_string(lives);
		debugFont->BlitText(50, 100, lifeString);

		// show score
		std::string scoreString = "Score: " + std::to_string(gameScore);
		int scoreStringWidth = debugFont->WidthText(scoreString);
		debugFont->BlitText(blit3D->screenWidth - scoreStringWidth - 50,
			100, scoreString);

		// show level and ready text
		if (nextLevelTimer > 0) {
			std::string levelString = "Level " + std::to_string(level + 1);
			int levelStringWidth = debugFont->WidthText(levelString);
			debugFont->BlitText(blit3D->screenWidth / 2 - levelStringWidth / 2, blit3D->screenHeight / 2 + 60, levelString);
			std::string readyString = "READY";
			int readyStringWidth = debugFont->WidthText(readyString);
			debugFont->BlitText(blit3D->screenWidth / 2 - readyStringWidth / 2, blit3D->screenHeight / 2 - 60, readyString);
		}

		// show go text
		if (goTimer > 0) {
			std::string goString = "GO!";
			int goStringWidth = debugFont->WidthText(goString);
			debugFont->BlitText(blit3D->screenWidth / 2 - goStringWidth / 2,
				blit3D->screenHeight / 2 - 30, goString);
		}
		break;
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		switch (gameState)
		{
		case START:
			flickerTimer = 0;
			nextLevelTimer = 2.f;
			gameScore = 0;
			Helper::ResetGame(level);
			Helper::StartGame();
			break;
		case GAME_OVER:
			flickerTimer = 0;
			nextLevelTimer = 2.f;
			level = 0;
			gameState = START;
			break;
		case PLAYING:
			if (pausedForNextRound) return;
			Helper::LaunchBall();
			break;
		}
	}
}

void DoCursor(double x, double y)
{
	//scale display size
	cursorX = static_cast<float>(x) * (blit3D->screenWidth / blit3D->trueScreenWidth);
}

void DoMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		switch (gameState)
		{
		case START:
			flickerTimer = 0;
			nextLevelTimer = 2.f;
			gameScore = 0;
			Helper::ResetGame(level);
			Helper::StartGame();
			flickerTimer = 0;
			break;
		case GAME_OVER:
			flickerTimer = 0;
			nextLevelTimer = 2.f;
			level = 0;
			gameState = START;
			break;
		case PLAYING:
			if (pausedForNextRound) return;
			Helper::LaunchBall();
			break;
		}
	}

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1920, 1080);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);
	blit3D->SetDoCursor(DoCursor);
	blit3D->SetDoMouseButton(DoMouseButton);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}
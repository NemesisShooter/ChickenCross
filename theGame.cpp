#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <assert.h>

bool InitEverything();
bool InitSDL();
bool CreateWindow();
bool CreateRenderer();

void SetupRenderer();
void Render();
void RunGame();
void EnemyAdd();
void WallAdd();
void CollectibleAdd();
void EnemyMove();
void RPlayerPosition();
void RCoinPosition();
void RCoinPositionOnce();

int GameEnd();
int DeadEnd();
int HeyCoin();
int ThatsAWall();

bool CheckCollision(const SDL_Rect &rect1, const SDL_Rect &rect2);
bool WallCollide(const SDL_Rect &rect1, const SDL_Rect &rect2);
bool CheckCollisionEnemy();
bool CheckCollisionWall();
bool CheckCollisionCollectible();

//Position of the window
int posX = 100;
int posY = 100;
int sizeX = 400;
int sizeY = 400;

//Ammount of pixels the player will be able to move with single key press
int movementFactor = 5;
int lastEnemyPosition = 50;
int lastCoinPosition = 75;
int lastWallPosition = 100;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Rect playerPos;
SDL_Rect topBar;
SDL_Rect bottomBar;

enum class Direction
{
	Left,
	Right
};

struct Enemy
{
	Enemy(SDL_Rect pos_, int speed_, Direction dir_)
	{
		pos = pos_;
		speed = speed_;
		dir = dir_;
	}
	SDL_Rect pos;
	int speed;
	Direction dir;
};
std::vector< Enemy >enemies;

struct Wall
{
	Wall(SDL_Rect pos_)
	{
		pos = pos_;
	}
	SDL_Rect pos;
};
std::vector< Wall >walls;

struct Collectible
{
	Collectible(SDL_Rect pos_)
	{
		pos = pos_;
	}
	SDL_Rect pos;
};
std::vector< Collectible >collectibles;

int main(int argc, char* args[])
{
	if (!InitEverything())
		return -1;
	EnemyAdd();
	EnemyAdd();

	WallAdd();
	WallAdd();

	CollectibleAdd();

	EnemyAdd();
	EnemyAdd();

	WallAdd();
	WallAdd();

	//Initialising top and bottom bar
	topBar.x = 0;
	topBar.y = 0;
	topBar.w = sizeX;
	topBar.h = 20;

	bottomBar.x = 0;
	bottomBar.y = sizeY - 20;
	bottomBar.w = sizeX;
	bottomBar.h = 20;

	//Initialising player
	playerPos.w = 20;
	playerPos.h = 20;
	RPlayerPosition();

	RunGame();
}

void RunGame()
{
	bool loop = true;
	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_RIGHT:
						playerPos.x += movementFactor;
						break;
					case SDLK_LEFT:
						playerPos.x -= movementFactor;
						break;
					case SDLK_DOWN:
						playerPos.y += movementFactor;
						break;
					case SDLK_UP:
						playerPos.y -= movementFactor;
						break;
					default:
						break;
				}
			}
		}
	EnemyMove();

	//Check collisions against enemies
	if (CheckCollisionEnemy())
		RPlayerPosition(),
		DeadEnd();

	//Check collision against coin
	if (CheckCollisionCollectible());

	//Check collision against wall
	if (CheckCollisionWall())
		ThatsAWall();

	//Check collision against top bar
	if (playerPos.y<(topBar.y+topBar.h))
		RPlayerPosition(),
		GameEnd(); 
	if (playerPos.y>(bottomBar.y+bottomBar.h))
		RPlayerPosition();
	Render();
	//Adding delay to make game run at 60 FPS
	SDL_Delay(16); 	
	}
}

void Render()
{	//Clearing the window first
	SDL_RenderClear (renderer);
	//Making background colour black
	SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
	//Rendering top and bottom bar
	SDL_RenderFillRect(renderer, &bottomBar);
	SDL_RenderFillRect(renderer, &topBar);

	//Changing colour to blue for player
	SDL_SetRenderDrawColor (renderer, 0, 0, 255, 255);
	//Rendering player
	SDL_RenderFillRect(renderer, &playerPos);

	//Changing colour to yellow for collectible
	SDL_SetRenderDrawColor (renderer, 255, 204, 0, 0);
	for(const auto &p:collectibles)
	SDL_RenderFillRect(renderer, &p.pos);

	//Changing colour to gray for walls
	SDL_SetRenderDrawColor (renderer, 61, 61, 61, 0);
	for(const auto &p:walls)
	SDL_RenderFillRect(renderer, &p.pos);
	
	//Changing colour to red for enemies
	SDL_SetRenderDrawColor (renderer, 255, 0, 0, 255);
	for(const auto &p:enemies)
	SDL_RenderFillRect(renderer, &p.pos);

	SDL_SetRenderDrawColor(renderer, 86, 0, 103, 0);
	SDL_RenderPresent(renderer);
}

bool InitEverything()
{
	if (!InitSDL())
		return false;
	if (!CreateWindow())
		return false;
	if (!CreateRenderer())
		return false;

	SetupRenderer();
	return true;
}

bool InitSDL()
{
 	if (SDL_Init(SDL_INIT_EVERYTHING)==-1)
	 {
	 	//If fail then print error
	 	std::cout <<"Failed to Initialize SDL:"<< SDL_GetError()<<std::endl;
	 	return -1;
	 }
	 return true;
}

bool CreateWindow()
{
	 	//Create and initialize window
	window = SDL_CreateWindow ("Game", posX, posY, sizeX, sizeY, 0);

	if (window == 0)
	{
		std::cout <<"Failed to create window"<< SDL_GetError();
		return -1;
	}
	return true;
}

bool CreateRenderer()
{
	//Create and init renderer
	renderer = SDL_CreateRenderer (window, -1, 0);
	if (renderer == 0)
	{
		std::cout <<"Failed to create renderer"<< SDL_GetError();
		return -1;
	}
	return true;
}

void SetupRenderer()
{
	//Setting size of renderer to be same as window
	SDL_RenderSetLogicalSize (renderer, sizeX, sizeY);
	SDL_SetRenderDrawColor (renderer, 255, 0, 0, 255);
}

void EnemyMove()
{
	for(auto &p:enemies)
	{
		if(p.dir == Direction::Right)
		{
			p.pos.x += p.speed;
			if (p.pos.x >= sizeX)
				p.pos.x = 0;
		}
		else
		{
			p.pos.x -= p.speed;
			if((p.pos.x + p.pos.w)<=0)
				p.pos.x = sizeX - p.pos.w;
		}
	}
}

bool CheckCollision(const SDL_Rect &rect1, const SDL_Rect &rect2)
{
	//finding edges of rect1
	int left1 = rect1.x;
	int right1 = rect1.x + rect1.w;
	int top1 = rect1.y;
	int bottom1 = rect1.y + rect1.h;
	
	//finding edges of rect2
	int left2 = rect2.x;
	int right2 = rect2.x + rect2.w;
	int top2 = rect2.y;
	int bottom2 = rect2.y + rect2.h;

	//checking edges
	if(left1>right2)
		return false;
	if(right1<left2)
		return false;
	if(top1>bottom2)
		return false;
	if(bottom1<top2)
		return false;
	return true;
}

bool WallCollide(const SDL_Rect &rect1, const SDL_Rect &rect2)
{
	//finding edges of rect1
	int left1 = rect1.x;
	int right1 = rect1.x + rect1.w;
	int top1 = rect1.y;
	int bottom1 = rect1.y + rect1.h;
	
	//finding edges of rect2
	int left2 = rect2.x;
	int right2 = rect2.x + rect2.w;
	int top2 = rect2.y;
	int bottom2 = rect2.y + rect2.h;

	bool noOverlap = right1 > left2 ||
                     right2 > left1 ||
                     top1 > bottom2 ||
                     top2 > bottom1;

    return !noOverlap;
}

bool CheckCollisionEnemy()
{
	for(const auto &p:enemies)
	{
		if(CheckCollision(p.pos, playerPos))
			return true;
	}
	return false;
}

bool CheckCollisionCollectible()
{
	for(const auto &p:collectibles)
	{
		if(CheckCollision(p.pos, playerPos))
			HeyCoin();
			return false;
	}
	return false;
}

bool CheckCollisionWall()
{
	for(const auto &p:walls)
	{
		if(WallCollide(p.pos, playerPos))
			return true;
	}
	return false;
}

void EnemyAdd()
{
	if((rand()%2)==0)
	{
		enemies.push_back(Enemy({rand()%300,lastEnemyPosition,20,20}, 1, Direction::Right));
	}
	else
	{
		enemies.push_back(Enemy({rand()%300,lastEnemyPosition,20,20}, 1, Direction::Left));
	}
	lastEnemyPosition += 25;
}

void WallAdd()
{
	if((rand()%2)==0)
	{
		walls.push_back(Wall({rand()%300,lastWallPosition,20,20}));
	}
	else
	{
		walls.push_back(Wall({rand()%300,lastWallPosition,20,20}));
	}
	lastWallPosition += 50;
}

void CollectibleAdd()
{
	if((rand()%2)==0)
	{
		collectibles.push_back(Collectible({rand()%300,lastCoinPosition,20,20}));
	}
	else
	{
		collectibles.push_back(Collectible({rand()%300,lastCoinPosition,20,20}));
	}
	lastCoinPosition += 25;
}

using namespace std;
int GameEnd() 
{ 
	cout << "You Won, Try Again?" <<endl;
}

using namespace std;
int HeyCoin() 
{ 
	cout << "Hey You Found A Coin!" <<endl;
	collectibles.clear();
	CollectibleAdd();
}

using namespace std;
int DeadEnd()
{ 
	cout << "Oops You Are Dead, Try Again?" <<endl;
}
 
using namespace std;
int ThatsAWall()
{ 
	cout << "NONONO thats a wall you are not allowed to touch it!" <<endl;
}

void RPlayerPosition()
{
	playerPos.x = ( sizeX / 2 ) - ( playerPos.w / 2 );
	playerPos.y = sizeY - bottomBar.h;
}
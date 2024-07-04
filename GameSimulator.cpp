#include "GameSimulator.h"
#include "Stick.h"
#include "ASSERT.h"

const float GameSimulator::mapLimitLeft = -1.0f;
const float GameSimulator::mapLimitRight = 1.0f;
const float GameSimulator::mapLimitUp = 1.0f;
const float GameSimulator::mapLimitDown = -1.0f;
const XMFLOAT3 GameSimulator::OBSTACLE_START_LOCATION = XMFLOAT3(0.3f, 0.3f, 0.5f);
GameSimulator::GameSimulator()
	: mUserCharactor(nullptr)
	, mGameLevel(eGameLevel::Normal)
{
	mGameObjects = new GameObject * [MAX_OBJECTS];
	mObstacles = new Obstacle * [MAX_OBSTACLES];
}
GameSimulator::~GameSimulator()
{
	delete[] mObstacles;
	delete[] mGameObjects;
}


bool GameSimulator::RegisterObstacle(Obstacle* const obstacle)
{
	if (mObstacleCount >= MAX_OBSTACLES)
	{
		return false;
	}
	mObstacles[mObstacleCount++] = obstacle;
	return true;
}
bool GameSimulator::RegisterUserCharactor(UserCharactor* const userCharactor)
{
	if (mUserCharactor != nullptr)
	{
		return false;
	}
	mUserCharactor = userCharactor;
	return true;
}
bool GameSimulator::Spawn(GameObject* const gameObject)
{
	if (mGameObjectCount >= MAX_OBJECTS)
	{
		return false;
	}
	gameObject->OnSpawn();
	mGameObjects[mGameObjectCount++] = gameObject;
	return true;
}
void GameSimulator::Release()
{
	for (GameObject** p = mGameObjects; p < mGameObjects + mGameObjectCount; ++p)
	{
		delete* p;
		*p = nullptr;
	}
	mObstacleCount = 0;
	delete mUserCharactor;

	mUserCharactor = nullptr;
}



void GameSimulator::Update()
{
	/////////////////////////////////////////////////////////
	////////hack! 게임 끝났을 때 어떻게 할지 정하자..////////
	/////////////////////////////////////////////////////////
	if (IsGameOver())
	{
		return;
	}
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////

	ASSERT(mObstacles != nullptr, "");
	for (Obstacle** p = mObstacles; p < mObstacles + mObstacleCount; ++p)
	{
		Obstacle* obstacle = *p;
		obstacle->Move();
	}
	ASSERT(mUserCharactor != nullptr, "");
	for (Obstacle** p = mObstacles; p < mObstacles + mObstacleCount; ++p)
	{
		Obstacle* obstacle = *p;
		if (mUserCharactor->IsConflict(obstacle))
		{
			mbGameOver = true;
		}
	}
	++mGameTick;
	if (mGameTick % 100 == 0)
	{
		Obstacle* newObstacle = new Obstacle(XMFLOAT3(0.3f, 0.3f, 0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f), 0.05f, mGameLevel);
		bool successSpawn = Spawn(newObstacle);
		if (successSpawn == false)
		{
			delete newObstacle;
		}
	}
}
void GameSimulator::InitGame()
{
	GameSimulator* gameSimulator = GameSimulator::GetInstance();
	Stick* leftEdge = new Stick(XMFLOAT3(-1.05f, 0.0f, 0.5f), XMFLOAT3(0.f, 0.f, 0.f), 0.1f, 2.2f);
	Stick* rightEdge = new Stick(XMFLOAT3(1.05f, 0.0f, 0.5f), XMFLOAT3(0.f, 0.f, 0.f), 0.1f, 2.2f);
	Stick* upEdge = new Stick(XMFLOAT3(0.0f, 1.05f, 0.5f), XMFLOAT3(0.f, 0.f, 0.f), 2.2f, 0.1f);
	Stick* downEdge = new Stick(XMFLOAT3(0.0f, -1.05f, 0.5f), XMFLOAT3(0.f, 0.f, 0.f), 2.2f, 0.1f);
	
	Obstacle* obstacle1 = new Obstacle(OBSTACLE_START_LOCATION, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.05f, mGameLevel);
	gameSimulator->Spawn(static_cast<GameObject*>(obstacle1));
	UserCharactor* userCharactor = new UserCharactor(XMFLOAT3(0.0f, 0.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f), 0.05f);
	gameSimulator->Spawn(static_cast<GameObject*>(userCharactor));
	
	gameSimulator->Spawn(static_cast<GameObject*>(leftEdge));
	gameSimulator->Spawn(static_cast<GameObject*>(rightEdge));
	gameSimulator->Spawn(static_cast<GameObject*>(upEdge));
	gameSimulator->Spawn(static_cast<GameObject*>(downEdge));

}
void GameSimulator::resetGame()
{
	GameSimulator* gameSimulator = GameSimulator::GetInstance();
	mGameObjectCount = 1;
	
	for (Obstacle** p = mObstacles; p < mObstacles + mObstacleCount; ++p)
	{
		(*p)->UnSpawn();
	}
	mObstacleCount = 0;
	mbGameOver = false;
	Obstacle* obstacle1 = new Obstacle(OBSTACLE_START_LOCATION, XMFLOAT3(1.0f, 0.0f, 0.0f), 0.05f, mGameLevel);
	gameSimulator->Spawn(static_cast<GameObject*>(obstacle1));
	mGameTick = 0;
}

void GameSimulator::ChangeGameLevel(const eGameLevel gameLevel)
{
	mGameLevel = gameLevel;
	for (Obstacle** p = mObstacles; p < mObstacles + mObstacleCount; ++p)
	{
		(*p)->SetSpeed(gameLevel);
	}
}



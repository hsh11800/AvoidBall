#pragma once
#include "Graphics.h"
#include "GameObject.h"
#include "UserCharactor.h"
#include "Obstacle.h"
#include "ShootingGame.h"
#include "eGameLevel.h"
class GameSimulator
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static GameSimulator* GetInstance();
	void InitGame();
	void Update();
	bool RegisterObstacle(Obstacle* const obstacle);
	bool RegisterUserCharactor(UserCharactor* const userCharactor);
	bool Spawn(GameObject* const gameObject);
	void Release();
	bool IsGameOver() const;
	void ChangeGameLevel(const eGameLevel gameLevel);
	UINT GetObstacleCount() const;
	

	enum
	{
		MAX_OBJECTS = 1000,
		MAX_OBSTACLES = 999,
		
	};
	static const float mapLimitLeft;
	static const float mapLimitRight;
	static const float mapLimitUp;
	static const float mapLimitDown;
private:
	GameSimulator();
	GameSimulator(const GameSimulator& other);
	~GameSimulator();
	void resetGame();
	GameSimulator& operator=(const GameSimulator rhs);
	GameObject** mGameObjects;
	UINT mGameObjectCount = 0;
	UserCharactor* mUserCharactor;
	Obstacle** mObstacles;
	UINT mObstacleCount = 0;
	bool mbGameOver = false;
	UINT mGameTick = 0;
	static const XMFLOAT3 OBSTACLE_START_LOCATION;
	eGameLevel mGameLevel;
};

inline GameSimulator* GameSimulator::GetInstance()
{
	static GameSimulator instance;
	return &instance;
}

inline bool GameSimulator::IsGameOver() const
{
	return mbGameOver;
}
inline UINT GameSimulator::GetObstacleCount() const
{
	return mObstacleCount;
}

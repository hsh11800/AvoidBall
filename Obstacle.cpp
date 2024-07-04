#include "Obstacle.h"
#include "GameSimulator.h"
#include "Graphics.h"

const FLOAT Obstacle::NORMAL_SPEED = 0.4f;
Obstacle::Obstacle(const XMFLOAT3 center, const XMFLOAT3 color, const float radius, const eGameLevel gameLevel)
	: CircleObject(center, color, radius)
	, mMoveDirection(makeRandomDirectionScalar(), makeRandomDirectionScalar(), 0)
{
	SetSpeed(gameLevel);
}
void Obstacle::Move()
{
	XMMATRIX translate = XMMatrixTranslation(mMoveDirection.x * mSpeed, mMoveDirection.y * mSpeed, mMoveDirection.z * mSpeed);
	mCb.MulWorldMatrix(translate);
	eLocationInMap locationInMap = isInMap();
	switch (locationInMap)
	{
	case eLocationInMap::LeftOver:
		mMoveDirection.x = (rand() % 100) / 400.f;
		break;
	case eLocationInMap::RightOver:
		mMoveDirection.x = -(rand() % 100) / 400.f;
		break;
	case eLocationInMap::UpOver:
		mMoveDirection.y = -(rand() % 100) / 400.f;
		break;
	case eLocationInMap::DownOver:
		mMoveDirection.y = (rand() % 100) / 400.f;
		break;
	case eLocationInMap::InMap:
		break;
	}
}
void Obstacle::registerInSimulator()
{
	GameSimulator* gameSimulator = GameSimulator::GetInstance();
	bool b = gameSimulator->RegisterObstacle(this);
	ASSERT(b,"");
}

void Obstacle::SetSpeed(eGameLevel gameLevel)
{
	switch (gameLevel)
	{
	case eGameLevel::Easy:
		mSpeed = 0.1f;
		break;
	case eGameLevel::Normal:
		mSpeed = NORMAL_SPEED;
		break;
	case eGameLevel::Hard:
		mSpeed = 1.0f;
		break;
	default:
		break;
	}
}



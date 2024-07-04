#pragma once
#include "CircleObject.h"
#include "eGameLevel.h"
class Obstacle :
    public CircleObject
{
public:
	Obstacle(const XMFLOAT3 center, const XMFLOAT3 color, const float radius, const eGameLevel gameLevel);
	void Move();
	void SetSpeed(eGameLevel gameLevel);
private:
	FLOAT makeRandomDirectionScalar() const;
	void registerInSimulator() override;
	
	XMFLOAT3 mMoveDirection;
	FLOAT mSpeed;

	const static FLOAT NORMAL_SPEED;

	
	
};

inline FLOAT Obstacle::makeRandomDirectionScalar() const
{
	return (rand() % 100) / 200.f - 0.25f;
}


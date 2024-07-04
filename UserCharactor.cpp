#include "UserCharactor.h"
#include "GameSimulator.h"
UserCharactor::UserCharactor(const XMFLOAT3 center, const XMFLOAT3 color, const float radius)
	: CircleObject(center,color,radius)
{

}
void UserCharactor::Move(FLOAT x, FLOAT y, FLOAT z)
{
	ConstantBuffer lastCb = mCb;
	XMMATRIX translateMatrix = XMMatrixTranslation(x, y, z);
	mCb.MulWorldMatrix(translateMatrix);
	if (isInMap() == eLocationInMap::InMap)
	{
		return;
	}
	mCb = lastCb;
}

void UserCharactor::registerInSimulator()
{
	GameSimulator* gameSimulator = GameSimulator::GetInstance();
	gameSimulator->RegisterUserCharactor(this);
}


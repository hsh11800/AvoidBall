#define _USE_MATH_DEFINES
#include "CircleObject.h"
#include "GameSimulator.h"
CircleObject::CircleObject(const XMFLOAT3 center, const XMFLOAT3 color, const float radius)
	: mCenter(center), mColor(color), mRadius(radius)
{

	indexBuffer_t centerVertexIndex = addVertex({ mCenter, mColor });
	indexBuffer_t lastVertexIndex;
	indexBuffer_t nowVertexIndex;
	FLOAT vertexX = center.x + mRadius;
	FLOAT vertexY = center.y;
	ASSERT((cos(0) - 1.0) < 0.00001 && (cos(0) - 1.0) > -0.00001, "first circle vertex is worng");
	ASSERT((sin(0) - 0.0) < 0.00001 && (sin(0) - 0.0) > -0.00001, "first circle vertex is worng");
	lastVertexIndex = addVertex({ XMFLOAT3(vertexX, vertexY, mCenter.z), mColor });

	for (int i = 1; i <= eCircleTriangleNum; ++i)
	{
		vertexX = center.x + mRadius * cosf(2 * static_cast<FLOAT>(M_PI) * (FLOAT)i / (FLOAT)eCircleTriangleNum);
		vertexY = center.y + mRadius * sinf(2 * static_cast<FLOAT>(M_PI) * (FLOAT)i / (FLOAT)eCircleTriangleNum);
		nowVertexIndex = addVertex({ XMFLOAT3(vertexX, vertexY, mCenter.z), mColor });

		addIndex(centerVertexIndex);
		addIndex(nowVertexIndex);
		addIndex(lastVertexIndex);


		lastVertexIndex = nowVertexIndex;
	}



}

eLocationInMap CircleObject::isInMap() const
{
	XMFLOAT3 worldCenter = mCb.MulWorldVector(mCenter);
	if (worldCenter.x + mRadius > GameSimulator::mapLimitRight)
	{
		return eLocationInMap::RightOver;
	}
	if (worldCenter.x - mRadius < GameSimulator::mapLimitLeft)
	{
		return eLocationInMap::LeftOver;
	}
	if (worldCenter.y + mRadius > GameSimulator::mapLimitUp)
	{
		return eLocationInMap::UpOver;
	}
	if (worldCenter.y - mRadius < GameSimulator::mapLimitDown)
	{
		return eLocationInMap::DownOver;
	}
	return eLocationInMap::InMap;

}

bool CircleObject::IsConflict(const CircleObject* const other) const
{
	XMFLOAT3 worldCenter = mCb.MulWorldVector(mCenter);
	XMFLOAT3 otherWorldCenter = other->mCb.MulWorldVector(other->mCenter);
	FLOAT diffX = worldCenter.x - otherWorldCenter.x;
	FLOAT diffY = worldCenter.y - otherWorldCenter.y;
	FLOAT diffZ = worldCenter.z - otherWorldCenter.z;



	FLOAT distance = sqrtf((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ));
	FLOAT radiusSum = mRadius + other->mRadius;
	if (distance <= radiusSum)
	{
		return true;
	}
	else
	{
		return false;
	}
}

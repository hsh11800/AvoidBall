#pragma once
#include "GameObject.h"
#include "eLocationInMap.h"
class CircleObject :
    public GameObject
{

public:
	CircleObject(const XMFLOAT3 center, const XMFLOAT3 color, const float radius);
	bool IsConflict(const CircleObject* const other) const;
	eLocationInMap isInMap() const;
private:
    XMFLOAT3 mCenter;
    XMFLOAT3 mColor;
    FLOAT mRadius;
    
    enum CircleTriangleNum
    {
        eCircleTriangleNum = 1000,
    };

};


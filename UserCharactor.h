#pragma once
#include "CircleObject.h"
class UserCharactor :
    public CircleObject
{
public:
	UserCharactor(const XMFLOAT3 center, const XMFLOAT3 color, const float radius);
	void Move(FLOAT x, FLOAT y, FLOAT z);
	
private:
	void registerInSimulator() override;
};


#pragma once
#include "GameObject.h"

class Stick :
	public GameObject
{

public:
	Stick(XMFLOAT3 center, XMFLOAT3 color, FLOAT width, FLOAT height);
	

	XMFLOAT3 GetCenter() const
	{
		return mCenter;
	}

	XMFLOAT3 GetColor() const 
	{
		return mColor;
	}

	FLOAT GetWidth() const 
	{
		return mWidth;
	}

	FLOAT GetHeight() const 
	{
		return mHeight;
	}

private:
	void registerInSimulator() override;

	XMFLOAT3 mCenter;
	XMFLOAT3 mColor;
	FLOAT mWidth;
	FLOAT mHeight;


};


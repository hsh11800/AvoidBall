#include "ConstantBuffer.h"


ConstantBuffer::ConstantBuffer(XMMATRIX world, XMMATRIX view, XMMATRIX projection)
{
	mWorld = world;
	mView = view;
	mProjection = projection;
}

void ConstantBuffer::SetProjection(XMMATRIX projection)
{
	mProjection = projection;
}



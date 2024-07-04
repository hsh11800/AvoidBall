#include "Stick.h"
#include "ASSERT.h"
Stick::Stick(XMFLOAT3 center, XMFLOAT3 color, FLOAT width, FLOAT height)
	: GameObject(), mCenter(center), mColor(color), mWidth(width), mHeight(height)
{
	//set stick vertex

	indexBuffer_t v1Index = addVertex({ XMFLOAT3(mCenter.x - mWidth / 2, mCenter.y + mHeight / 2, 0.5f),mColor });//ÁÂ»ó
	ASSERT(GetVertexCount() == 1, "vertex count need to be 1");
	indexBuffer_t v2Index = addVertex({ XMFLOAT3(mCenter.x + mWidth / 2, mCenter.y + mHeight / 2, 0.5f),mColor });//¿ì»ó
	ASSERT(GetVertexCount() == 2, "vertex count need to be 2");
	indexBuffer_t v3Index = addVertex({ XMFLOAT3(mCenter.x + mWidth / 2, mCenter.y - mHeight / 2, 0.5f),mColor });//¿ìÇÏ
	indexBuffer_t v4Index = addVertex({ XMFLOAT3(mCenter.x - mWidth / 2, mCenter.y - mHeight / 2, 0.5f),mColor });//ÁÂÇÏ

	addIndex(v1Index);
	addIndex(v2Index);
	addIndex(v3Index);

	addIndex(v2Index);
	addIndex(v3Index);
	addIndex(v4Index);

	addIndex(v3Index);
	addIndex(v4Index);
	addIndex(v1Index);

	addIndex(v4Index);
	addIndex(v1Index);
	addIndex(v2Index);



}
void Stick::registerInSimulator()
{
	return;
}
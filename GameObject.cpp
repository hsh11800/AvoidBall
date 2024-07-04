#include "GameObject.h"
extern HWND g_hWnd;
GameObject::GameObject()
{
	mVertices = new VertexWithColor[MAX_VERTEX_COUNT];
	mIndices = new indexBuffer_t[MAX_INDEX_COUNT];

	XMMATRIX world = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	
	// Initialize the projection matrix
//#define PROJECTION_TEST 1
#ifdef PROJECTION_TEST
	Graphics* graphics = Graphics::GetInstance(nullptr, nullptr);
	RECT rc = graphics->GetWindowRect();
#else
	RECT rc;
	GetClientRect(g_hWnd, &rc);
#endif
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;


	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / (FLOAT)height, 0.01f, 100.0f); // hack! 

	mCb = ConstantBuffer(world, view, projection);

}

GameObject::~GameObject()
{
	delete[] mVertices;
	delete[] mIndices;
}

/// <summary>
/// 
/// </summary>
/// <param name="vertex"></param>
/// <returns>추가한 vertex의 배열 내 index값</returns>
indexBuffer_t GameObject::addVertex(const VertexWithColor vertex)
{
	ASSERT(mVertexCount < MAX_VERTEX_COUNT, "buffer overflow");
	mVertices[mVertexCount] = vertex;
	++mVertexCount;
	return mVertexCount - 1;
}

/// <summary>
/// 
/// </summary>
/// <param name="index"></param>
/// <returns>추가한 index의 배열 내 index</returns>
UINT GameObject::addIndex(const indexBuffer_t index)
{
	ASSERT(mIndexCount < MAX_INDEX_COUNT, "buffer overflow");
	mIndices[mIndexCount++] = index;
	return mIndexCount - 1;
}

UINT GameObject::GetVertexCount() const
{
	return mVertexCount;
}
UINT GameObject::GetIndexCount() const
{
	return mIndexCount;
}
const VertexWithColor* GameObject::GetVertics() const
{
	return mVertices;
}
const indexBuffer_t* GameObject::GetIndices() const
{
	return mIndices;
}
ConstantBuffer GameObject::GetConstantBuffer() const
{
	return mCb;
}

void GameObject::OnSpawn()
{
	Graphics* graphics = Graphics::GetInstance(nullptr, nullptr);
	graphics->AddGameObject(this);

	registerInSimulator();

}

void GameObject::UnSpawn()
{
	Graphics* graphics = Graphics::GetInstance(nullptr, nullptr);
	graphics->RemoveGameObject(this);
}

void GameObject::ResetProjection()
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / (FLOAT)height, 0.01f, 100.0f); // hack! 

	mCb.SetProjection(projection);

	
}


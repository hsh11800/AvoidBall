#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include "resource.h"
#include "Graphics.h"
#include "VertexWithColor.h"
#include "ConstantBuffer.h"
#include "GraphicsType.h"
#include "eLocationInMap.h"
class GameObject
{
public :
	GameObject();
	virtual ~GameObject();
	UINT GetVertexCount() const;
	UINT GetIndexCount() const;
	const VertexWithColor* GetVertics() const;
	const indexBuffer_t* GetIndices() const;
	ConstantBuffer GetConstantBuffer() const;
	void ResetProjection();
	

	void OnSpawn();
	void UnSpawn();
	enum 
	{
		MAX_VERTEX_COUNT = 10000,
		MAX_INDEX_COUNT = 100000,
	};

protected :
	indexBuffer_t addVertex(const VertexWithColor vertex);
	UINT addIndex(const indexBuffer_t index);
	ConstantBuffer mCb;
private :
	virtual void registerInSimulator() = 0;
	VertexWithColor* mVertices;
	indexBuffer_t mVertexCount = 0;
	indexBuffer_t* mIndices;
	UINT mIndexCount = 0;
	
};



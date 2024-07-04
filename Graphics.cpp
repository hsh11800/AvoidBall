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
#include "GameSimulator.h"
Graphics::Graphics(HINSTANCE hInst, HWND hWnd)
	: mHInst(hInst)
	, mHWnd(hWnd)
	, mMaxGameObjectcount(GameSimulator::MAX_OBJECTS) 
	, mMaxVertexCount(mMaxGameObjectcount * 2000)	/*hack : 나중에 바꿀것*/
	, mMaxIndexCount(mMaxVertexCount * 4)			/*hack : 나중에 바꿀것*/
{
	mGameObjects = new GameObject * [mMaxGameObjectcount];
	mGameObjectVertexStartIndices = new UINT[mMaxGameObjectcount];
	mGameObjectVertexStartIndices[0] = 0;
	mGameObjectIndexStartIndices = new UINT[mMaxGameObjectcount];
	mGameObjectIndexStartIndices[0] = 0;
	mVertices = new VertexWithColor[mMaxVertexCount];
	mIndices = new indexBuffer_t[mMaxIndexCount];


	initDevice();

}


Graphics::~Graphics()
{
	cleanupDevice();

	for (GameObject** p = mGameObjects; p < mGameObjects + mGameObjectCount; ++p)
	{
		delete* p;
	}
	delete[] mGameObjects;
	delete[] mGameObjectVertexStartIndices;
	delete[] mGameObjectIndexStartIndices;
	delete[] mVertices;
	delete[] mIndices;
}



/// <summary>
/// 
/// </summary>
/// <param name="gameObject"> heap memory object</param>
void Graphics::AddGameObject(GameObject* gameObject)
{
	ASSERT(mGameObjectCount < mMaxGameObjectcount, "buffer overflow");
	mGameObjects[mGameObjectCount++] = gameObject;
	
	UINT vertexCount = gameObject->GetVertexCount();
	UINT lastVertexStartIndex = mGameObjectVertexStartIndices[mGameObjectCount - 1];
	mGameObjectVertexStartIndices[mGameObjectCount] = lastVertexStartIndex + vertexCount;

	const VertexWithColor* vertices = gameObject->GetVertics();
	ASSERT(lastVertexStartIndex + vertexCount < mMaxVertexCount, "");
	memcpy(mVertices + lastVertexStartIndex, vertices, sizeof(VertexWithColor) * vertexCount);
	mVertexCount += vertexCount;
	UINT indexCount = gameObject->GetIndexCount();
	UINT lastIndexStartIndex = mGameObjectIndexStartIndices[mGameObjectCount - 1];
	mGameObjectIndexStartIndices[mGameObjectCount] = lastIndexStartIndex + indexCount;
	mIndexCount += indexCount;

	const indexBuffer_t* indices = gameObject->GetIndices();
	ASSERT(lastIndexStartIndex + indexCount < mMaxIndexCount, "");
	memcpy(mIndices + lastIndexStartIndex, indices, sizeof(indexBuffer_t) * indexCount);


}

bool Graphics::RemoveGameObject(GameObject* gameObject)
{
	for (GameObject** pObject = mGameObjects; pObject < mGameObjects + mGameObjectCount; ++pObject)
	{
		if (*pObject == gameObject)
		{
			const UINT gameObjectIndex = pObject - mGameObjects;
			const UINT objectVertexCount = gameObject->GetVertexCount();
			const UINT objectIndexCount = gameObject->GetIndexCount();

			if (gameObjectIndex == mGameObjectCount - 1)
			{
				--mGameObjectCount;
				mVertexCount -= objectVertexCount;
				mIndexCount -= objectIndexCount;

				delete gameObject;
				return true;
			}
			// mGameObjects에서 gameObject 제거
			for (; pObject + 1 < mGameObjects + mGameObjectCount; ++pObject)
			{
				*pObject = *(pObject + 1);
			}
			//없어진 object의 vertex위치로 vertices 땡기기
			UINT vertexStartIndex = mGameObjectVertexStartIndices[gameObjectIndex];
			for (VertexWithColor* pVertex = mVertices + vertexStartIndex;
									pVertex + objectVertexCount < mVertices + mVertexCount;
									++pVertex)
			{
				*pVertex = *(pVertex + objectVertexCount);
			}
			
			//object별 vertices에서 시작 index "값"을 땡기기
			if (gameObjectIndex == 0)
			{
				mGameObjectVertexStartIndices[0] = 0;
			}
			else
			{
				const UINT prevVertexCount = mGameObjects[gameObjectIndex - 1]->GetVertexCount();
				mGameObjectVertexStartIndices[gameObjectIndex] = mGameObjectVertexStartIndices[gameObjectIndex - 1] + prevVertexCount;
			}
			
			for (UINT i = gameObjectIndex + 1; i < mGameObjectCount - 1; ++i)
			{
				ASSERT(i >= 1, "out of range");
				const UINT prevVertexCount = mGameObjects[i - 1]->GetVertexCount();
				mGameObjectVertexStartIndices[i] = mGameObjectVertexStartIndices[i - 1] + prevVertexCount;
			}

			
			//없어진 object의 index위치로 indices 땡기기
			UINT indexStartIndex = mGameObjectIndexStartIndices[gameObjectIndex];
			
			for (indexBuffer_t* pIndex = mIndices + indexStartIndex;
				pIndex + objectIndexCount < mIndices + mIndexCount;
				++pIndex)
			{
				*pIndex = *(pIndex + objectIndexCount);
			}

			//object별 indices에서 시작 index "값"을 땡기기
			if (gameObjectIndex == 0)
			{
				mGameObjectIndexStartIndices[0] = 0;
			}
			else
			{
				const UINT prevIndexCount = mGameObjects[gameObjectIndex - 1]->GetIndexCount();
				mGameObjectIndexStartIndices[gameObjectIndex] = mGameObjectIndexStartIndices[gameObjectIndex - 1] + prevIndexCount;

			}
			for (UINT i = gameObjectIndex + 1; i < mGameObjectCount - 1; ++i)
			{
				ASSERT(i >= 1, "out of range");
				const UINT prevIndexCount = mGameObjects[i - 1]->GetIndexCount();
				mGameObjectIndexStartIndices[i] = mGameObjectIndexStartIndices[i - 1] + prevIndexCount;
			}


			--mGameObjectCount;
			mVertexCount -= objectVertexCount;
			mIndexCount -= objectIndexCount;

			delete gameObject;

			return true;
		}

	}
	//game object not be in array  
	return false;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT Graphics::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Graphics::initDevice()
{
	HRESULT hr = S_OK;

	
	ASSERT(mHWnd != NULL, "mHWnd need set in initWindow");
	
	GetClientRect(mHWnd, &mRc);
	UINT width = mRc.right - mRc.left;
	UINT height = mRc.bottom - mRc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mHWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, mDriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &mSwapChain, &mD3dDevice, &mFeatureLevel, &mImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = mD3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &mRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &vp);

	// Compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"ShootingGame.fx", "VS", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = mD3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &mVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR" , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = mD3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;

	// Set the input layout
	mImmediateContext->IASetInputLayout(mVertexLayout);

	// Compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"ShootingGame.fx", "PS", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = mD3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &mPixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return hr;


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	//create constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = mD3dDevice->CreateBuffer(&bd, NULL, &mConstantBuffer);


	if (FAILED(hr))
		return hr;

	ASSERT(mD3dDevice != nullptr, " initDevice(): mD3dDevice need not null");

	initRender();

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void Graphics::cleanupDevice()
{
	if (mImmediateContext) mImmediateContext->ClearState();

	if (mConstantBuffer) mConstantBuffer->Release();
	if (mVertexBuffer) mVertexBuffer->Release();
	if (mIndexBuffer) mIndexBuffer->Release();
	if (mVertexLayout) mVertexLayout->Release();
	if (mVertexShader) mVertexShader->Release();
	if (mPixelShader) mPixelShader->Release();
	if (mDepthStencil) mDepthStencil->Release();
	if (mDepthStencilView) mDepthStencilView->Release();
	if (mRenderTargetView) mRenderTargetView->Release();
	if (mSwapChain) mSwapChain->Release();
	if (mImmediateContext) mImmediateContext->Release();
	if (mD3dDevice) mD3dDevice->Release();
}




//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Graphics::Render()
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 1.0f, 1.f, 1.f, 1.0f }; //red, green, blue, alpha
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

	//
	// Clear the depth buffer to 1.0 (max depth)
	//
	//mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//
	// Update variables for the first cube
	//

	mImmediateContext->UpdateSubresource(mVertexBuffer, 0, NULL, mVertices, 0, 0);
	mImmediateContext->UpdateSubresource(mIndexBuffer, 0, NULL, mIndices, 0, 0);

	for (GameObject** p = mGameObjects; p < mGameObjects + mGameObjectCount; ++p)
	{
		const GameObject* gameObject = *p;
		ConstantBuffer cb = gameObject->GetConstantBuffer();
		ConstantBuffer cbTranspose = cb;
		cbTranspose.MakeTranspose();

		mImmediateContext->UpdateSubresource(mConstantBuffer, 0, NULL, &cbTranspose, 0, 0);
		//이거 이용해서 vertex, index도 바꿀수 있는거 아닐까 ?
		//
		// Render the first cube
		//
		mImmediateContext->VSSetShader(mVertexShader, NULL, 0);
		mImmediateContext->VSSetConstantBuffers(0, 1, &mConstantBuffer);
		mImmediateContext->PSSetShader(mPixelShader, NULL, 0);

		UINT drawedIndexCount = gameObject->GetIndexCount();
		UINT startIndexLocation = mGameObjectIndexStartIndices[p - mGameObjects ];
		INT baseVertexLocation = mGameObjectVertexStartIndices[p -mGameObjects ];
		mImmediateContext->DrawIndexed(drawedIndexCount, startIndexLocation, baseVertexLocation);

	}

	mSwapChain->Present(0, 0);
}

void Graphics::initRender()
{
	if (mVertexBuffer) mVertexBuffer->Release();
	if (mIndexBuffer) mIndexBuffer->Release();


	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexWithColor) * mMaxVertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mVertices;
	hr = mD3dDevice->CreateBuffer(&bd, &InitData, &mVertexBuffer);
	if (FAILED(hr))
		return;

	// Set vertex buffer
	UINT stride = sizeof(VertexWithColor);
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	//// Set primitive topology
	//mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//set index buffer

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(indexBuffer_t) * mMaxIndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	//D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = mIndices;
	hr = mD3dDevice->CreateBuffer(&bd, &InitData, &mIndexBuffer);
	if (FAILED(hr))
		return;

	// Set index buffer
	mImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST); 


}



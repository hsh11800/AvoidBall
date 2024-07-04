#pragma once
#include "GameObject.h"
#include "GraphicsType.h"
#include "VertexWithColor.h"
#include "ASSERT.h"
class GameObject;
class Graphics
{
public:
	static Graphics* GetInstance(HINSTANCE hInst, HWND hWnd);

	void AddGameObject(GameObject* gameObject);
	bool RemoveGameObject(GameObject* gameObject);
	void Render();
	RECT GetWindowRect() const;
private:
	Graphics(HINSTANCE hInst, HWND hWnd);
	Graphics(const Graphics& other);
	~Graphics();
	void cleanupDevice();
	Graphics& operator=(const Graphics& rhs);
	
	void initRender();
	HRESULT initDevice();
	static HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	
	GameObject** mGameObjects;
	UINT mMaxGameObjectcount;
	UINT mGameObjectCount = 0;
	UINT* mGameObjectVertexStartIndices;
	UINT* mGameObjectIndexStartIndices;

	VertexWithColor* mVertices;
	indexBuffer_t* mIndices;
	UINT mMaxVertexCount;
	UINT mMaxIndexCount;
	UINT mVertexCount = 0;
	UINT mIndexCount = 0;


	/// <summary>
	/// directx pipeline에 필요한 것들
	/// </summary>
	HINSTANCE               mHInst = NULL;
	HWND                    mHWnd = NULL;
	D3D_DRIVER_TYPE         mDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       mFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device* mD3dDevice = NULL;
	ID3D11DeviceContext* mImmediateContext = NULL;
	IDXGISwapChain* mSwapChain = NULL;
	ID3D11RenderTargetView* mRenderTargetView = NULL;
	ID3D11Texture2D* mDepthStencil = NULL;
	ID3D11DepthStencilView* mDepthStencilView = NULL;
	ID3D11VertexShader* mVertexShader = NULL;
	ID3D11PixelShader* mPixelShader = NULL;
	ID3D11InputLayout* mVertexLayout = NULL;
	ID3D11Buffer* mVertexBuffer = NULL;
	ID3D11Buffer* mIndexBuffer = NULL;
	ID3D11Buffer* mConstantBuffer = NULL;
	RECT mRc;
};

inline Graphics* Graphics::GetInstance(HINSTANCE hInst, HWND hWnd)
{
	static Graphics instance(hInst, hWnd);
	ASSERT(instance.mHInst != nullptr, "");
	ASSERT(instance.mHWnd != nullptr, "");
	return &instance;
}

inline RECT Graphics::GetWindowRect() const
{
	return mRc;
}


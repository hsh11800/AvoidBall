#pragma once
#include <windows.h>
#include <xnamath.h>
#include "ASSERT.h"
class ConstantBuffer
{
private:
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

public:
	ConstantBuffer(XMMATRIX world, XMMATRIX view, XMMATRIX projection);
	ConstantBuffer() {}
	XMMATRIX GetWorld() const;
	XMMATRIX GetView() const;
	XMMATRIX GetProjection() const;


	void SetProjection(XMMATRIX projection);

	/// <summary>
	/// mWorld = mWorld * matrix
	/// </summary>
	/// <param name="matrix"></param>
	void MulWorldMatrix(const XMMATRIX matrix);

	/// <summary>
	/// mView = mView * matrix
	/// </summary>
	/// <param name="matrix"></param>
	void MulViewMatrix(const XMMATRIX matrix);

	/// <summary>
	/// mProjection = mProjection * matrix
	/// </summary>
	/// <param name="matrix"></param>
	void MulProjectionMatrix(const XMMATRIX matrix);

	void MakeTranspose();

	XMFLOAT3 MulWorldVector(const XMFLOAT3 vec) const;
	XMFLOAT3 MulViewVector(const XMFLOAT3 vec) const;
	XMFLOAT3 MulProjectionVector(const XMFLOAT3 vec) const;

	static XMFLOAT3 MulMatrixVector(const XMMATRIX matrix, const XMFLOAT3 vec);
};

/// <summary>
/// transpose world,view,projection matrix for send dx11 shader
/// </summary>
inline void ConstantBuffer::MakeTranspose()
{
	mWorld = XMMatrixTranspose(mWorld);
	mView = XMMatrixTranspose(mView);
	mProjection = XMMatrixTranspose(mProjection);

}

inline XMMATRIX ConstantBuffer::GetWorld() const
{
	return mWorld;
}
inline XMMATRIX ConstantBuffer::GetView() const
{
	return mView;
}
inline XMMATRIX ConstantBuffer::GetProjection() const
{
	return mProjection;
}
inline void ConstantBuffer::MulWorldMatrix(const XMMATRIX matrix)
{
	mWorld = mWorld * matrix;

}
inline void ConstantBuffer::MulViewMatrix(const XMMATRIX matrix)
{
	mView *= matrix;

}
inline void ConstantBuffer::MulProjectionMatrix(const XMMATRIX matrix)
{
	mProjection *= matrix;
}


inline XMFLOAT3 ConstantBuffer::MulWorldVector(const XMFLOAT3 vec) const
{
	return MulMatrixVector(mWorld, vec);

}

inline XMFLOAT3 ConstantBuffer::MulViewVector(const XMFLOAT3 vec) const
{
	return MulMatrixVector(mView, vec);
}
inline XMFLOAT3 ConstantBuffer::MulProjectionVector(const XMFLOAT3 vec) const
{
	return MulMatrixVector(mProjection, vec);
}
inline XMFLOAT3 ConstantBuffer::MulMatrixVector(const XMMATRIX matrix, const XMFLOAT3 vec)
{
	XMVECTOR vW;
	vW.m128_f32[0] = vec.x;
	vW.m128_f32[1] = vec.y;
	vW.m128_f32[2] = vec.z;
	vW.m128_f32[3] = 1.0f;

	XMVECTOR vX = _mm_shuffle_ps(vW, vW, _MM_SHUFFLE(0, 0, 0, 0));
	XMVECTOR vY = _mm_shuffle_ps(vW, vW, _MM_SHUFFLE(1, 1, 1, 1));
	XMVECTOR vZ = _mm_shuffle_ps(vW, vW, _MM_SHUFFLE(2, 2, 2, 2));
	vW = _mm_shuffle_ps(vW, vW, _MM_SHUFFLE(3, 3, 3, 3));
	// Perform the opertion on the first row
	vX = _mm_mul_ps(vX, matrix.r[0]);
	vY = _mm_mul_ps(vY, matrix.r[1]);
	vZ = _mm_mul_ps(vZ, matrix.r[2]);
	vW = _mm_mul_ps(vW, matrix.r[3]);
	// Perform a binary add to reduce cumulative errors
	vX = _mm_add_ps(vX, vZ);
	vY = _mm_add_ps(vY, vW);
	vX = _mm_add_ps(vX, vY);
	XMFLOAT3 result(vX.m128_f32[0], vX.m128_f32[1], vX.m128_f32[2]);
	return result;
}



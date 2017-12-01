#pragma once

#include "commonincludes.h"
#include "WaterShader.h"
#include "Camera.h"

ALIGN16 class CWater sealed
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		SVertex( ) = default;
		SVertex( DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT2 Tex ) :
			Position( Pos ), Texture( Tex ) {};
	};
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::shared_ptr<CWaterShader> mShader;

	std::vector<SVertex> mVertices;
	std::vector<DWORD> mIndices;

	UINT mIndexCount;
	UINT mVertexCount;

	DirectX::XMMATRIX mWorld;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CWater( ) = default;
	CWater( ID3D11Device * Device, ID3D11DeviceContext * Context,
		std::shared_ptr<CWaterShader> Shader, float radius, UINT numQuads,
		UINT repeatTexture );
	~CWater( );
public:
	void Update( float frameTime, CCamera * camera );
	void Render( DirectX::FXMMATRIX& view, DirectX::FXMMATRIX& projection );
private:
	void InitializeBuffers( float radius, UINT numQuads, UINT repeatTexture );
public:
	inline void * operator new( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete( void* object )
	{
		return _aligned_free( object );
	};
};

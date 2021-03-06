#pragma once


#include "commonincludes.h"
#include "ShaderHelper.h"
#include "GeometryHelper.h"
#include "SkyPlaneShader.h"


ALIGN16 class CloudPlane
{
public:
	typedef GeometryGenerator::SVertex SVertex;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::shared_ptr<SkyPlaneShader> mShader;
	SkyPlaneShader::STextureInfo mTextureInfo;

	GeometryGenerator::MeshData mPlane;
	DirectX::XMMATRIX mWorld;

	DirectX::XMFLOAT2 mVelocity;

	std::unique_ptr<CTexture> mClouds;
	std::unique_ptr<CTexture> mPerturb;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CloudPlane( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyPlaneShader> Shader );
	~CloudPlane( );
public:
	void Update( float frameTime );
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, DirectX::XMFLOAT3 const& CamPos );
public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	}
};


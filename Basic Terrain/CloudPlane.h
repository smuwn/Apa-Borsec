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

	GeometryGenerator::MeshData mPlane;
	DirectX::XMMATRIX mWorld;

	SkyPlaneShader::STextureInfo mFirstTextureInfo;
	SkyPlaneShader::STextureInfo mSecondTextureInfo;

	DirectX::XMFLOAT2 mFirstTextureSpeed;
	DirectX::XMFLOAT2 mSecondTextureSpeed;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CloudPlane( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyPlaneShader> Shader );
	~CloudPlane( );
public:
	void Update( DirectX::XMFLOAT3 const& CamPos, float frameTime );
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
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


#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "SkyShader.h"
#include "GeometryHelper.h"
#include "CloudPlane.h"


ALIGN16 class Skydome
{
public:
	typedef GeometryGenerator::SVertex SVertex;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::shared_ptr<SkyShader> mShader;

	std::unique_ptr<CloudPlane> mClouds;

	GeometryGenerator::MeshData mSphere;

	DirectX::XMMATRIX mWorld;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	Skydome( ) = delete;
	Skydome( ID3D11Device * Device, ID3D11DeviceContext * Context,
		std::shared_ptr<SkyShader> Shader, std::shared_ptr<SkyPlaneShader> CloudShader );
	~Skydome( );
public:
	void Update( DirectX::XMFLOAT3 const& CamPos, float frameTime );
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
public:
	inline void* operator new( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete( void* object )
	{
		_aligned_free( object );
	}
};


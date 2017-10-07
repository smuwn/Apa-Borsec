#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "SkyShader.h"
#include "GeometryHelper.h"


ALIGN16 class Skybox
{
public:
	typedef GeometryGenerator::SVertex SVertex;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::shared_ptr<SkyShader> mShader;

	GeometryGenerator::MeshData mSphere;

	DirectX::XMMATRIX mWorld;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	Skybox( ) = delete;
	Skybox( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyShader> Shader );
	~Skybox( );
private:
	void LoadModel( LPWSTR lpPath );
public:
	void Update( DirectX::XMFLOAT3 const& CamPos );
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


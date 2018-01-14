#pragma once


#include "commonincludes.h"
#include "ParticleShader.h"
#include "Camera.h"
#include "Texture.h"



ALIGN16 class ParticleSystem
{
	typedef CParticleShader::SPerFrame SInfo;
	typedef CParticleShader::SVertex SVertex;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mStreamOutBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mRenderingBuffer;

	ID3D11ShaderResourceView * mTexture;

#if DEBUG || _DEBUG
	Microsoft::WRL::ComPtr<ID3D11Buffer> mDebugBuffer;
#endif

	SInfo mInformations;

	std::shared_ptr<CParticleShader> mShader;

	bool mFirstTime;
	UINT mMaxParticles;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

public:
	ParticleSystem( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<CParticleShader> Shader,
		UINT MaxParticles = 10000 );
	~ParticleSystem( );
public:
	void Update( float frameTime );
	void Render( CCamera * camera );
public:
	inline void SetEmitDir( DirectX::XMFLOAT3 Dir ) { mInformations.gEmitDirW = Dir; };
	inline void SetEmitPos( DirectX::XMFLOAT3 Pos ) { mInformations.gEmitPosW = Pos; };
	inline void SetTexture( ID3D11ShaderResourceView * SRV ) { mTexture = SRV; };
private:
	void CreateBuffers( );
public:
	inline void * operator new( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete( void * object )
	{
		_aligned_free( object );
	}
};


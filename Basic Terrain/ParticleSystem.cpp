#include "ParticleSystem.h"



ParticleSystem::ParticleSystem( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<CParticleShader> Shader,
	UINT MaxParticles ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader ),
	mMaxParticles( MaxParticles )
{
	try
	{
		CreateBuffers( );
		ZeroVariable( mInformations );
		mFirstTime = true;
	}
	CATCH;
}


ParticleSystem::~ParticleSystem( )
{
}


void ParticleSystem::Update( float frameTime )
{
	mInformations.gSystemLifeTime += frameTime;
	mInformations.gDeltaTime = frameTime;
}

void ParticleSystem::Render( CCamera * camera )
{
	static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	static UINT Stride = sizeof( SVertex );
	static UINT Offset = 0;

	mInformations.gEyePosW = camera->GetCamPos( );
	mInformations.gViewProj = DirectX::XMMatrixTranspose( camera->GetView( ) * camera->GetProjection( ) );

	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	if ( mFirstTime )
		mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offset );
	else
		mContext->IASetVertexBuffers( 0, 1, mRenderingBuffer.GetAddressOf( ), &Stride, &Offset );

	ID3D11Buffer * targets[ ] = { mStreamOutBuffer.Get( ) };
	mContext->SOSetTargets( 1, targets, &Offset );

	if ( mFirstTime )
	{
		mShader->RenderStreamOut( mInformations, 1 );
		mFirstTime = false;
	}
	else
		mShader->RenderStreamOut( mInformations );

#if DEBUG || _DEBUG
	//mContext->CopyResource( mDebugBuffer.Get( ), mStreamOutBuffer.Get( ) );

	//
	//DX::ThrowIfFailed(
	//	mContext->Map( mDebugBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_READ, 0, &mappedSubresource )
	//	);

	//SVertex * vertices = ( SVertex* ) mappedSubresource.pData;
	//for ( int i = 0; i < 10; ++i )
	//{
	//	DX::OutputVDebugString( L"Particle %d: Age: %.2f, Type: %d\n", i, vertices[ i ].Age, vertices[ i ].Type );
	//}

	//mContext->Unmap( mDebugBuffer.Get( ), 0 );
#endif


	std::swap( mRenderingBuffer, mStreamOutBuffer );

	ID3D11Buffer * nullTargets[ ] = { nullptr };
	mContext->SOSetTargets( 1, nullTargets, &Offset );

	mContext->IASetVertexBuffers( 0, 1, mRenderingBuffer.GetAddressOf( ), &Stride, &Offset );

	mShader->Render( mTexture );
}

void ParticleSystem::CreateBuffers( )
{
	
	SVertex Emitter;
	Emitter.Age = 1.0f;
	Emitter.Type = 0;

	ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof( SVertex ) * 1, 0, ( void* ) &Emitter );

	ShaderHelper::CreateBuffer( mDevice, &mStreamOutBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT,
		D3D11_BIND_FLAG( D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_FLAG::D3D11_BIND_STREAM_OUTPUT ),
		sizeof( SVertex ) * mMaxParticles, 0 );

	ShaderHelper::CreateBuffer( mDevice, &mRenderingBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT,
		D3D11_BIND_FLAG( D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_FLAG::D3D11_BIND_STREAM_OUTPUT ),
		sizeof( SVertex ) * mMaxParticles, 0 );
	
	ShaderHelper::CreateBuffer( mDevice, &mDebugBuffer,
		D3D11_USAGE::D3D11_USAGE_STAGING,
		D3D11_BIND_FLAG( 0 ), sizeof( SVertex ) * mMaxParticles, D3D11_CPU_ACCESS_READ );
	
}
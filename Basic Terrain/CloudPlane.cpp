#include "CloudPlane.h"



CloudPlane::CloudPlane( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyPlaneShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		GeometryGenerator::CreateCurbedPlane( 10, 6, 0.5f, -3.0f, 4, mPlane );
		
		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mPlane.Vertices.size( ), 0, &mPlane.Vertices[ 0 ] );
		ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof( DWORD ) * mPlane.Indices.size( ), 0, &mPlane.Indices[ 0 ] );

		mWorld = DirectX::XMMatrixIdentity( );

		mClouds = std::make_unique<CTexture>( ( LPWSTR ) L"Data/cloud001.dds", mDevice );
		mPerturb = std::make_unique<CTexture>( ( LPWSTR ) L"Data/perturb001.dds", mDevice );

		mTextureInfo.brightness = 0.65f;
		mTextureInfo.scale = 0.3f;
		mTextureInfo.translation = DirectX::XMFLOAT2( 0.0f, 0.0f );

		mVelocity.x = 0.01f;
		mVelocity.y = 0.005f;
	}
	CATCH;
}


CloudPlane::~CloudPlane( )
{
	mClouds.reset( );
	mPerturb.reset( );

	mVertexBuffer.Reset( );
	mIndexBuffer.Reset( );

	mShader.reset( );
}


void CloudPlane::Update( float frameTime )
{
	mTextureInfo.translation.x += mVelocity.x * frameTime;
	mTextureInfo.translation.y += mVelocity.y * frameTime;

	if ( mTextureInfo.translation.x > 1.0f )
		mTextureInfo.translation.x -= 1.0f;
	if ( mTextureInfo.translation.y > 1.0f )
		mTextureInfo.translation.y -= 1.0f;
}

void CloudPlane::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, DirectX::XMFLOAT3 const& CamPos )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mWorld = DirectX::XMMatrixTranslation( CamPos.x, CamPos.y, CamPos.z );
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	mContext->RSSetState( DX::FrontCulling.Get( ) );
	mShader->Render( mPlane.Indices.size( ), mTextureInfo, mWorld, View, Projection, mClouds.get( ), mPerturb.get( ) );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
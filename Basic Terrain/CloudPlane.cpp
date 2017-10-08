#include "CloudPlane.h"



CloudPlane::CloudPlane( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyPlaneShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		GeometryGenerator::CreateCurbedPlane( 10, 10, 0.5f, -0.5f, 4, mPlane );
		
		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mPlane.Vertices.size( ), 0, &mPlane.Vertices[ 0 ] );
		ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof( DWORD ) * mPlane.Indices.size( ), 0, &mPlane.Indices[ 0 ] );

		mWorld = DirectX::XMMatrixIdentity( );
		
		mFirstTextureSpeed = DirectX::XMFLOAT2( 0.05f, 0.02f );
		mSecondTextureSpeed = DirectX::XMFLOAT2( 0.07f, 0.03f );

		mFirstTextureInfo.Texture = new CTexture( L"Data/cloud001.dds", mDevice );
		mFirstTextureInfo.Offset = DirectX::XMFLOAT2( 0.0f, 0.0f );
		mSecondTextureInfo.Texture = new CTexture( L"Data/cloud002.dds", mDevice );
		mSecondTextureInfo.Offset = DirectX::XMFLOAT2( 0.0f, 0.0f );
	}
	CATCH;
}


CloudPlane::~CloudPlane( )
{
	if ( mFirstTextureInfo.Texture )
	{
		delete mFirstTextureInfo.Texture;
		mFirstTextureInfo.Texture = 0;
	}
	if ( mSecondTextureInfo.Texture )
	{
		delete mSecondTextureInfo.Texture;
		mSecondTextureInfo.Texture = 0;
	}

	mVertexBuffer.Reset( );
	mIndexBuffer.Reset( );

	mShader.reset( );
}


void CloudPlane::Update( DirectX::XMFLOAT3 const& CamPos, float frameTime )
{
	mWorld = DirectX::XMMatrixTranslation( CamPos.x, CamPos.y, CamPos.z );
	
	mFirstTextureInfo.Offset.x += mFirstTextureSpeed.x * frameTime;
	mFirstTextureInfo.Offset.y += mFirstTextureSpeed.y * frameTime;

	mSecondTextureInfo.Offset.x += mSecondTextureSpeed.x * frameTime;
	mSecondTextureInfo.Offset.y += mSecondTextureSpeed.y * frameTime;

	if ( mFirstTextureInfo.Offset.x >= 1.0f )
		mFirstTextureInfo.Offset.x -= 1.0f;
	if ( mFirstTextureInfo.Offset.y >= 1.0f )
		mFirstTextureInfo.Offset.y -= 1.0f;

	if ( mSecondTextureInfo.Offset.x >= 1.0f )
		mSecondTextureInfo.Offset.x -= 1.0f;
	if ( mSecondTextureInfo.Offset.y >= 1.0f )
		mSecondTextureInfo.Offset.y -= 1.0f;
}

void CloudPlane::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	mContext->RSSetState( DX::NoCulling.Get( ) );
	mContext->OMSetDepthStencilState( DX::DS2D.Get( ), 0 );
	mShader->Render( mPlane.Indices.size( ), 0.85f, mWorld, View, Projection, mFirstTextureInfo, mSecondTextureInfo );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
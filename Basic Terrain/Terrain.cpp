#include "Terrain.h"



CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitTerrain( );
		InitBuffers( );
	}
	CATCH;
}

CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader, LPWSTR Heightmap ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitBuffers( );
	}
	CATCH;
}

CTerrain::~CTerrain( )
{
	mShader.reset( );

	mVertexBuffer.Reset( );
	mIndexBuffer.Reset( );
}

void CTerrain::InitTerrain( )
{
	mRowCount = 50;
	mColCount = 50;


	mIndexCount = ( mRowCount - 1 ) * ( mColCount - 1 ) * 6;
	mVertexCount = ( mRowCount - 1 ) * ( mColCount - 1 ) * 4;

	int VertexCount = 0;
	int IndexCount = 0;

	float X, Z;

	for ( size_t i = 0; i < ( mRowCount - 1 ); ++i )
	{
		for ( size_t j = 0; j < ( mColCount - 1 ); ++j )
		{

			// Lower-right
			X = ( float ) j + 1 - mRowCount / 2;
			Z = ( float ) i + 1 - mColCount / 2;
			mVertices.emplace_back( X, 0.0f, Z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			// Upper-right
			X = ( float ) j + 1 - mRowCount / 2;
			Z = ( float ) i - mColCount / 2;
			mVertices.emplace_back( X, 0.0f, Z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			// Upper-left
			X = ( float ) j - mRowCount / 2;
			Z = ( float ) i - mColCount / 2;
			mVertices.emplace_back( X, 0.0f, Z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			mIndices.emplace_back( VertexCount - 3 );
			IndexCount++;
			mIndices.emplace_back( VertexCount - 1 );
			IndexCount++;
			// Lower-left
			X = ( float ) j - mRowCount / 2;
			Z = ( float ) i + 1 - mColCount / 2;
			mVertices.emplace_back( X, 0.0f, Z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

		}
	}

	assert( VertexCount == mVertexCount );
	assert( IndexCount == mIndexCount );
}

void CTerrain::InitBuffers( )
{
	ShaderHelper::CreateBuffer( mDevice, mVertexBuffer.GetAddressOf( ),
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof( SVertex ) * mVertices.size( ), 0, &mVertices[ 0 ] );
	ShaderHelper::CreateBuffer( mDevice, mIndexBuffer.GetAddressOf( ),
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof( DWORD ) * mIndices.size( ), 0, &mIndices[ 0 ] );
}

void CTerrain::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->RSSetState( DX::Wireframe.Get( ) );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	mShader->Render( mIndexCount, DirectX::XMMatrixIdentity( ), View, Projection );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
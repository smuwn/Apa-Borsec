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

CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader, LPSTR Heightmap ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitHeightmap( Heightmap );
		InitHeightmapTerrain( );
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

void CTerrain::InitHeightmap( LPSTR Path )
{
	FILE * HeightmapFile;
	int error;
	
	error = fopen_s( &HeightmapFile, Path, "rb" );
	if ( error )
		throw std::exception( "Can't open heightmap file" );

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread( &fileHeader, sizeof( BITMAPFILEHEADER ), 1, HeightmapFile );
	fread( &infoHeader, sizeof( BITMAPINFOHEADER ), 1, HeightmapFile );

	mRowCount = infoHeader.biHeight;
	mColCount = infoHeader.biWidth;

	int imagesize = mRowCount * mColCount * 3;
	unsigned char * image = new unsigned char[ imagesize ];

	fseek( HeightmapFile, fileHeader.bfOffBits, SEEK_SET );

	fread( image, 1, imagesize, HeightmapFile );

	fclose( HeightmapFile );

	int k = 0; 
	float height;
	mHeightmap.resize( mRowCount * mColCount );

	for ( int i = 0; i < mRowCount; ++i )
	{
		for ( int j = 0; j < mColCount; ++j )
		{
			height = image[ k ];

			int Index = mColCount * i + j;

			mHeightmap[ Index ].x = ( float ) j;
			mHeightmap[ Index ].y = height / 15.0f;
			mHeightmap[ Index ].z = ( float ) i;

			k += 3;
		}
	}

	delete[ ] image;
}

void CTerrain::InitHeightmapTerrain( )
{
	mIndexCount = ( mRowCount - 1 ) * ( mColCount - 1 ) * 6;
	mVertexCount = ( mRowCount - 1 ) * ( mColCount - 1 ) * 4;

	int VertexCount = 0;
	int IndexCount = 0;

	for ( size_t i = 0; i < ( mRowCount - 1 ); ++i )
	{
		for ( size_t j = 0; j < ( mColCount - 1 ); ++j )
		{
			int index1 = mColCount * i + j; // Upper-left
			int index2 = mColCount * i + ( j + 1 ); // Upper-right
			int index3 = mColCount * ( i + 1 ) + ( j + 1 ); // Lower-right
			int index4 = mColCount * ( i + 1 ) + j; // Lower-left

			// Lower-right
			mVertices.emplace_back( mHeightmap[ index3 ].x, mHeightmap[ index3 ].y, mHeightmap[ index3 ].z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			// Upper-right
			mVertices.emplace_back( mHeightmap[ index2 ].x, mHeightmap[ index2 ].y, mHeightmap[ index2 ].z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			// Upper-left
			mVertices.emplace_back( mHeightmap[ index1 ].x, mHeightmap[ index1 ].y, mHeightmap[ index1 ].z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;

			mIndices.emplace_back( VertexCount - 3 );
			IndexCount++;
			mIndices.emplace_back( VertexCount - 1 );
			IndexCount++;

			// Lower-left
			mVertices.emplace_back( mHeightmap[ index4 ].x, mHeightmap[ index4 ].y, mHeightmap[ index4 ].z );
			mIndices.emplace_back( VertexCount );
			VertexCount++, IndexCount++;
		}
	}
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
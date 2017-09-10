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

CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader, 
	LPSTR Heightmap, LPSTR Normalmap ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitHeightmap( Heightmap );
		InitHeightmapTerrain( );
		InitNormals( Normalmap );
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

	for ( size_t i = 0; i < mRowCount; ++i )
	{
		for ( size_t j = 0; j < mColCount; ++j )
		{
			height = image[ k ];

			int Index = mColCount * i + j;

			mHeightmap[ Index ].x = ( float ) j - mColCount / 2;
			mHeightmap[ Index ].y = height / HeightFactor;
			mHeightmap[ Index ].z = ( float ) i - mRowCount / 2;

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
	mRowCount = 10;
	mColCount = 10;


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

void CTerrain::InitNormals( LPSTR Normalmap )
{
	FILE * Heightmap;
	int error;
	error = fopen_s( &Heightmap, Normalmap, "rb" );
	if ( error )
	{ // Couldn't open file
		std::vector<DirectX::XMFLOAT3> unnormalized;
		for ( size_t i = 0; i < mIndices.size( ) / 3; ++i )
		{
			float vecX, vecY, vecZ;
			vecX = mVertices[ mIndices[ ( i * 3 ) + 1 ] ].Position.x - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.x;
			vecY = mVertices[ mIndices[ ( i * 3 ) + 1 ] ].Position.y - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.y;
			vecZ = mVertices[ mIndices[ ( i * 3 ) + 1 ] ].Position.z - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.z;
			DirectX::XMVECTOR edge1 = DirectX::XMVectorSet( vecX, vecY, vecZ, 0.0f );


			vecX = mVertices[ mIndices[ ( i * 3 ) + 2 ] ].Position.x - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.x;
			vecY = mVertices[ mIndices[ ( i * 3 ) + 2 ] ].Position.y - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.y;
			vecZ = mVertices[ mIndices[ ( i * 3 ) + 2 ] ].Position.z - mVertices[ mIndices[ ( i * 3 ) + 0 ] ].Position.z;
			DirectX::XMVECTOR edge2 = DirectX::XMVectorSet( vecX, vecY, vecZ, 0.0f );

			DirectX::XMFLOAT3 Normal;
			DirectX::XMStoreFloat3( &Normal, DirectX::XMVector3Cross( edge1, edge2 ) );
			unnormalized.push_back( Normal );
		}

		DirectX::XMVECTOR sumNormal;
		int facesUsing;
		float tX = 0, tY = 0, tZ = 0;
		for ( size_t i = 0; i < mVertices.size( ); ++i )
		{
			facesUsing = 0;
			sumNormal = DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
			for ( size_t j = 0; j < mIndices.size( ) / 3; ++j )
			{
				if ( mIndices[ ( j * 3 ) + 0 ] == i ||
					mIndices[ ( j * 3 ) + 1 ] == i ||
					mIndices[ ( j * 3 ) + 2 ] == i )
				{
					tX = DirectX::XMVectorGetX( sumNormal ) + unnormalized[ j ].x;
					tY = DirectX::XMVectorGetY( sumNormal ) + unnormalized[ j ].y;
					tZ = DirectX::XMVectorGetZ( sumNormal ) + unnormalized[ j ].z;

					sumNormal = DirectX::XMVectorSet( tX, tY, tZ, 0.0f );
					facesUsing++;
				}
			}
			sumNormal = DirectX::XMVectorDivide( sumNormal,
				DirectX::XMVectorSet(
					( float ) facesUsing, ( float ) facesUsing, ( float ) facesUsing, ( float ) facesUsing
					) );

			DirectX::XMStoreFloat3( &mVertices[ i ].Normal, sumNormal );
		}
	}
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
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	mShader->Render( mIndexCount, DirectX::XMMatrixIdentity( ), View, Projection );
}
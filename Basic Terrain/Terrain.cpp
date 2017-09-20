#include "Terrain.h"



CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitTerrain( );
		//InitBuffers( );
		mTexture = std::make_shared<CTexture>( ( LPWSTR ) L"Data/Dirt01.dds", mDevice );
		DirectX::XMStoreFloat4x4( &mWorld, DirectX::XMMatrixIdentity( ) );
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
		//InitBuffers( );
		mTexture = std::make_shared<CTexture>( ( LPWSTR ) L"Data/Dirt01.dds", mDevice );
		DirectX::XMStoreFloat4x4( &mWorld, DirectX::XMMatrixIdentity( ) );
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

	mFileHeader = fileHeader;
	mInfoHeader = infoHeader;

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

	for (UINT i = 0; i < mRowCount; ++i )
	{
		for (UINT j = 0; j < mColCount; ++j )
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
	int FaceCount = ( mRowCount - 1 ) * ( mColCount - 1 ) * 2;
	mVertexCount = ( mRowCount ) * ( mColCount );

	mVertices.resize( mVertexCount );
	for (UINT i = 0; i < mRowCount; ++i )
		for (UINT j = 0; j < mColCount; ++j )
		{
			int index = i * mColCount + j;
			mVertices[ index ].Position.x = ( float ) j;
			mVertices[ index ].Position.z = ( float ) i;
			mVertices[ index ].Position.y = mHeightmap[ index ].y;
		}
	mIndexCount = FaceCount * 3;
	mIndices.resize( mIndexCount );

	int index = 0;

	float tU = 0, tV = 0;
	float VincrementValue = TextureRepeat / mRowCount;
	float UincrementValue = TextureRepeat / mColCount;

	for (UINT i = 0; i < mRowCount - 1; ++i )
	{
		for (UINT j = 0; j < mColCount - 1; ++j )
		{
			mIndices[ index + 0 ] = ( i + 1 ) * mColCount + j; // Bottom left
			mVertices[ ( i + 1 ) * mColCount + j ].Texture = DirectX::XMFLOAT2( tU,tV + VincrementValue );
			
			mIndices[ index + 1 ] = ( i + 1 ) * mColCount + j + 1; // Bottom right
			mVertices[ ( i + 1 ) * mColCount + j + 1 ].Texture = DirectX::XMFLOAT2( tU + UincrementValue, tV + VincrementValue );

			mIndices[ index + 2 ] = i * mColCount + j; // Top left
			mVertices[ i * mColCount + j ].Texture = DirectX::XMFLOAT2( tU, tV );

			mIndices[ index + 3 ] = ( i + 1 ) * mColCount + j + 1; // Bottom right

			mIndices[ index + 4 ] = i * mColCount + j + 1; // Top right
			mVertices[ i * mColCount + j + 1 ].Texture = DirectX::XMFLOAT2( tU + UincrementValue, tV );

			mIndices[ index + 5 ] = i * mColCount + j; // Top left

			tU += UincrementValue;

			index += 6;
		}
		tU = 0.0f;
		tV += VincrementValue;
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
			sumNormal = DirectX::XMVector3Normalize( sumNormal );
			sumNormal = DirectX::XMVectorDivide( sumNormal,
				DirectX::XMVectorSet(
					( float ) facesUsing, ( float ) facesUsing, ( float ) facesUsing, ( float ) facesUsing
					) );

			DirectX::XMStoreFloat3( &mVertices[ i ].Normal, sumNormal );
		}
		FILE * NormalmapFile;
		fopen_s( &NormalmapFile, Normalmap, "wb" );

		//fwrite( &mFileHeader, sizeof( mFileHeader ), 1, NormalmapFile );
		//fwrite( &mInfoHeader, sizeof( mInfoHeader ), 1, NormalmapFile );

		for ( size_t i = 0; i < mVertices.size( ); ++i )
		{
			float red = mVertices[ i ].Normal.x * 255;
			float green = mVertices[ i ].Normal.y * 255;
			float blue = mVertices[ i ].Normal.z * 255;

			fwrite( &red, sizeof( decltype(red) ), 1, NormalmapFile );
			fwrite( &green, sizeof( decltype( green ) ), 1, NormalmapFile );
			fwrite( &blue, sizeof( decltype( blue ) ), 1, NormalmapFile );
			
		}

		fclose( NormalmapFile );
	}
	else
	{ // There is a normal map file
		for ( size_t i = 0; i < mVertices.size( ); ++i )
		{
			float red, green, blue;
			fread( &red, sizeof( decltype( red ) ), 1, Heightmap );
			fread( &green, sizeof( decltype( green ) ), 1, Heightmap );
			fread( &blue, sizeof( decltype( blue ) ), 1, Heightmap );
			mVertices[ i ].Normal.x = red / 255.f;
			mVertices[ i ].Normal.y = green / 255.f;
			mVertices[ i ].Normal.z = blue / 255.f;
		}
		
		fclose( Heightmap );
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

void CTerrain::CopyVertices( void * To )
{
	DirectX::XMVECTOR Position;
	SVertex * ToAddress = ( SVertex* ) To;
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	for ( size_t i = 0; i < mVertices.size( ); ++i )
	{
		SVertex Vertex = mVertices[ i ];
		Position = DirectX::XMLoadFloat3( &Vertex.Position );
		Position = DirectX::XMVector3TransformCoord( Position, World );
		DirectX::XMStoreFloat3( &Vertex.Position, Position );
		ToAddress[ i ] = Vertex;
	}
	

}

void CTerrain::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, bool bWireframe )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	if ( bWireframe )
		mContext->RSSetState( DX::Wireframe.Get( ) );
	mShader->Render( mIndexCount, DirectX::XMMatrixIdentity( ), View, Projection,
		mTexture.get( ) );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}

void CTerrain::Identity( )
{
	DirectX::XMStoreFloat4x4( &mWorld, DirectX::XMMatrixIdentity( ) );
}

void CTerrain::RotateX( float Theta )
{
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	World *= DirectX::XMMatrixRotationX( Theta );
	DirectX::XMStoreFloat4x4( &mWorld, World );
}

void CTerrain::RotateY( float Theta )
{
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	World *= DirectX::XMMatrixRotationY( Theta );
	DirectX::XMStoreFloat4x4( &mWorld, World );
}

void CTerrain::RotateZ( float Theta )
{
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	World *= DirectX::XMMatrixRotationZ( Theta );
	DirectX::XMStoreFloat4x4( &mWorld, World );
}

void CTerrain::Translate( float X, float Y, float Z )
{
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	World *= DirectX::XMMatrixTranslation( X, Y, Z );
	DirectX::XMStoreFloat4x4( &mWorld, World );
}

void CTerrain::Scale( float X, float Y, float Z )
{
	DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4( &mWorld );
	World *= DirectX::XMMatrixScaling( X, Y, Z );
	DirectX::XMStoreFloat4x4( &mWorld, World );
}

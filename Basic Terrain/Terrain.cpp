#include "Terrain.h"




CTerrain::CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<C3DShader> Shader, 
	LPSTR Heightmap, LPSTR Normalmap, LPSTR Colormap,
	LPSTR Materials, LPSTR Blendingmap ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		mHasBlendingmap = false;
		InitHeightmap( Heightmap, Colormap );
		if ( Materials != "" && Blendingmap != "" )
		{
			mHasBlendingmap = true;
			CalculateNormalsFromHeightmap( );
			InitMaterials( Materials );
			InitMaterialMap( Blendingmap );
			InitMaterialBuffers( );
			InitHeightmapTerrain( );
			InitNormals( Normalmap );
			InitBuffers( );
		}
		else
		{
			InitHeightmapTerrain( );
			InitNormals( Normalmap );
			InitBuffers( );
		}
		mGrass = std::make_shared<CTexture>( ( LPWSTR ) L"Data/Dirt01.dds", mDevice );
		mSlope = std::make_shared<CTexture>( ( LPWSTR ) L"Data/slope.dds", mDevice );
		mRock = std::make_shared<CTexture>( ( LPWSTR ) L"Data/rock.dds", mDevice );
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

void CTerrain::InitHeightmap( LPSTR Path, LPSTR Colormap )
{
	FILE * HeightmapFile;
	FILE * ColormapFile;
	int error;
	
	error = fopen_s( &HeightmapFile, Path, "rb" );
	if ( error )
		throw std::exception( "Can't open heightmap file" );

	error = fopen_s( &ColormapFile, Colormap, "rb" );
	if ( error )
		throw std::exception( "Can't open colormap file" );

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread( &fileHeader, sizeof( BITMAPFILEHEADER ), 1, HeightmapFile );
	fread( &infoHeader, sizeof( BITMAPINFOHEADER ), 1, HeightmapFile );

	mRowCount = infoHeader.biHeight;
	mColCount = infoHeader.biWidth;

	fread( &fileHeader, sizeof( BITMAPFILEHEADER ), 1, ColormapFile );
	fread( &infoHeader, sizeof( BITMAPINFOHEADER ), 1, ColormapFile );

	assert( mRowCount == infoHeader.biHeight );
	assert( mColCount == infoHeader.biWidth );

	int imagesize = mRowCount * mColCount * 3;
	unsigned char * image = new unsigned char[ imagesize ];
	unsigned char * colors = new unsigned char[ imagesize ];

	fseek( HeightmapFile, fileHeader.bfOffBits, SEEK_SET );
	fseek( ColormapFile, fileHeader.bfOffBits, SEEK_SET );

	fread( image, 1, imagesize, HeightmapFile );
	fread( colors, 1, imagesize, ColormapFile );

	fclose( HeightmapFile );
	fclose( ColormapFile );

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
			mHeightmap[ Index ].r = colors[ k ] / 255.0f;
			mHeightmap[ Index ].g = colors[ k + 1 ] / 255.0f;
			mHeightmap[ Index ].b = colors[ k + 2 ] / 255.0f;

			k += 3;
		}
	}

	delete[ ] image;
	delete[ ] colors;
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
			mVertices[ index ].Position.x = mHeightmap[ index ].x;
			mVertices[ index ].Position.z = mHeightmap[ index ].z;
			mVertices[ index ].Position.y = mHeightmap[ index ].y;
			mVertices[ index ].Color.x = mHeightmap[ index ].r;
			mVertices[ index ].Color.y = mHeightmap[ index ].g;
			mVertices[ index ].Color.z = mHeightmap[ index ].b;
			mVertices[ index ].Color.w = 1.0f;
		}
	mIndexCount = FaceCount * 3;
	mIndices.resize( mIndexCount );

	int index = 0;

	float tU = 0, tV = 0;
	float VincrementValue = TextureRepeat / mRowCount;
	float UincrementValue = TextureRepeat / mColCount;
	float tDU = 0, tDV = 0;
	float DVincrementValue = ( TextureRepeat * 4 ) / mRowCount;
	float DUincrementValue = ( TextureRepeat * 4 ) / mColCount;

	for (UINT i = 0; i < mRowCount - 1; ++i )
	{
		for (UINT j = 0; j < mColCount - 1; ++j )
		{
			mIndices[ index + 0 ] = ( i + 1 ) * mColCount + j; // Bottom left
			mVertices[ ( i + 1 ) * mColCount + j ].Texture = DirectX::XMFLOAT4( tU, tV + VincrementValue, tDU, tDV + DVincrementValue );
			
			mIndices[ index + 1 ] = ( i + 1 ) * mColCount + j + 1; // Bottom right
			mVertices[ ( i + 1 ) * mColCount + j + 1 ].Texture = DirectX::XMFLOAT4( tU + UincrementValue, tV + VincrementValue, tDU + DUincrementValue, tDV + DVincrementValue);

			mIndices[ index + 2 ] = i * mColCount + j; // Top left
			mVertices[ i * mColCount + j ].Texture = DirectX::XMFLOAT4( tU, tV, tDU, tDV );

			mIndices[ index + 3 ] = ( i + 1 ) * mColCount + j + 1; // Bottom right

			mIndices[ index + 4 ] = i * mColCount + j + 1; // Top right
			mVertices[ i * mColCount + j + 1 ].Texture = DirectX::XMFLOAT4( tU + UincrementValue, tV, tDU + DUincrementValue, tDV );

			mIndices[ index + 5 ] = i * mColCount + j; // Top left

			tU += UincrementValue;
			tDU += DUincrementValue;

			index += 6;
		}
		tU = 0.0f;
		tV += VincrementValue;

		tDU = 0.0f;
		tDV += DVincrementValue;
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

void CTerrain::InitMaterials( LPSTR Materials )
{
	WCHAR Path[ MAX_PATH ];
	size_t Length = 0;
	mbstowcs_s( &Length, Path, Materials, MAX_PATH );

	std::wifstream ifMaterials( Path );

	if ( !ifMaterials.is_open( ) )
		throw std::exception( "Couldn't open Material Info file" );

	wchar_t ch = 0;

	int TextureCount, MaterialCount;

	while ( ch != ':' )
	{
		ch = ifMaterials.get( );
	}

	ifMaterials >> TextureCount;
	mMaterialTextures.resize( TextureCount );
	for ( int i = 0; i < TextureCount; ++i )
	{
		ch = ifMaterials.get( );
		while ( ch != ':' )
			ch = ifMaterials.get( );
		std::wstring Path;
		ifMaterials >> Path;
		try
		{
			mMaterialTextures[ i ] = std::make_shared<CTexture>( ( LPWSTR ) Path.c_str( ), mDevice );
		}
		CATCH;
	}

	ch = ifMaterials.get( );
	while ( ch != ':' )
		ch = ifMaterials.get( );
	ifMaterials >> MaterialCount;
	mMaterials.resize( MaterialCount );
	for ( int i = 0; i < MaterialCount; ++i )
	{
		ch = ifMaterials.get( );
		while ( ch != ':' )
			ch = ifMaterials.get( );
		ifMaterials >> mMaterials[ i ].texture1 >> mMaterials[ i ].texture2
			>> mMaterials[ i ].alphamap >> mMaterials[ i ].red
			>> mMaterials[ i ].green >> mMaterials[ i ].blue;
	}

	ifMaterials.close( );
}

void CTerrain::InitMaterialMap( LPSTR Materialmap )
{
	FILE * BlendingmapFile;
	int error;
	error = fopen_s( &BlendingmapFile, Materialmap, "rb" );
	if ( error )
		throw std::exception( "Couldn't open material map" );

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread( &fileHeader, sizeof( BITMAPFILEHEADER ), 1, BlendingmapFile );
	fread( &infoHeader, sizeof( BITMAPINFOHEADER ), 1, BlendingmapFile );

	assert( mRowCount == infoHeader.biHeight );
	assert( mColCount == infoHeader.biWidth );

	int imagesize = mRowCount * mColCount * 3;
	unsigned char * image = new unsigned char[ imagesize ];

	fseek( BlendingmapFile, fileHeader.bfOffBits, SEEK_SET );

	fread( image, 1, imagesize, BlendingmapFile );

	fclose( BlendingmapFile );

	int k = 0;
	for ( UINT i = 0; i < mRowCount; ++i )
	{
		for ( UINT j = 0; j < mColCount; ++j )
		{
			int Index = mColCount * i + j;
			mHeightmap[ Index ].rIndex = image[ k + 2 ];
			mHeightmap[ Index ].gIndex = image[ k + 1 ];
			mHeightmap[ Index ].bIndex = image[ k + 0 ];
			k += 3;
		}
	}

	delete[ ] image;
}

void CTerrain::InitMaterialBuffers( )
{
	for ( UINT i = 0; i < mRowCount - 1; ++i )
	{
		for ( UINT j = 0; j < mColCount - 1; ++j )
		{
			UINT bottomLeftIndex = ( i + 1 ) * mColCount + j;
			UINT bottomRightIndex = ( i + 1 ) * mColCount + j + 1;
			UINT topLeftIndex = i * mColCount + j;
			UINT topRightIndex = i * mColCount + j + 1;

			SHeightmap indexHeightmap = mHeightmap[ topLeftIndex ];
			size_t materialIndex = 0;
			for ( ; materialIndex < mMaterials.size( ); ++materialIndex )
			{
				if ( mMaterials[ materialIndex ].red == indexHeightmap.rIndex &&
					mMaterials[ materialIndex ].green == indexHeightmap.gIndex &&
					mMaterials[ materialIndex ].blue == indexHeightmap.bIndex )
					break;
			}
			assert( materialIndex != mMaterials.size( ) );

			SVertex Vertex;

			// Bottom left
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ bottomLeftIndex ].x, mHeightmap[ bottomLeftIndex ].y, mHeightmap[ bottomLeftIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ bottomLeftIndex ].nx, mHeightmap[ bottomLeftIndex ].ny, mHeightmap[ bottomLeftIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ bottomLeftIndex ].r, mHeightmap[ bottomLeftIndex ].g, mHeightmap[ bottomLeftIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );

			// Bottom right
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ bottomRightIndex ].x, mHeightmap[ bottomRightIndex ].y, mHeightmap[ bottomRightIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ bottomRightIndex ].nx, mHeightmap[ bottomRightIndex ].ny, mHeightmap[ bottomRightIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ bottomRightIndex ].r, mHeightmap[ bottomRightIndex ].g, mHeightmap[ bottomRightIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );

			// Top left
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ topLeftIndex ].x, mHeightmap[ topLeftIndex ].y, mHeightmap[ topLeftIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ topLeftIndex ].nx, mHeightmap[ topLeftIndex ].ny, mHeightmap[ topLeftIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ topLeftIndex ].r, mHeightmap[ topLeftIndex ].g, mHeightmap[ topLeftIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );

			// Bottom right
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ bottomRightIndex ].x, mHeightmap[ bottomRightIndex ].y, mHeightmap[ bottomRightIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ bottomRightIndex ].nx, mHeightmap[ bottomRightIndex ].ny, mHeightmap[ bottomRightIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ bottomRightIndex ].r, mHeightmap[ bottomRightIndex ].g, mHeightmap[ bottomRightIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );

			// Top right
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ topRightIndex ].x, mHeightmap[ topRightIndex ].y, mHeightmap[ topRightIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ topRightIndex ].nx, mHeightmap[ topRightIndex ].ny, mHeightmap[ topRightIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ topRightIndex ].r, mHeightmap[ topRightIndex ].g, mHeightmap[ topRightIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 0.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );

			// Top left
			Vertex.Position = DirectX::XMFLOAT3( mHeightmap[ topLeftIndex ].x, mHeightmap[ topLeftIndex ].y, mHeightmap[ topLeftIndex ].z );
			Vertex.Normal = DirectX::XMFLOAT3( mHeightmap[ topLeftIndex ].nx, mHeightmap[ topLeftIndex ].ny, mHeightmap[ topLeftIndex ].nz );
			Vertex.Color = DirectX::XMFLOAT4( mHeightmap[ topLeftIndex ].r, mHeightmap[ topLeftIndex ].g, mHeightmap[ topLeftIndex ].b, 1.0f );
			Vertex.Texture = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			mMaterials[ materialIndex ].Vertices.push_back( Vertex );
		}
	}
	for ( size_t i = 0; i < mMaterials.size( ); ++i )
	{
		if ( mMaterials[ i ].Vertices.size( ) > 0 )
		{
			ShaderHelper::CreateBuffer( mDevice, &mMaterials[ i ].VertexBuffer,
				D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
				sizeof( SVertex ) * mMaterials[ i ].Vertices.size( ), 0, &mMaterials[ i ].Vertices[ 0 ] );
		}
	}
}

void CTerrain::CalculateNormalsFromHeightmap( )
{
	using namespace DirectX;
	int index1, index2, index3, index;
	XMVECTOR V1, V2, V3, Edge1, Edge2, Sum;
	//XMFLOAT3 *Normals = new XMFLOAT3[ ( mRowCount - 1 ) * ( mColCount - 1 ) ];
	std::vector<XMFLOAT3> Normals;
	Normals.resize( ( mRowCount - 1 ) * ( mColCount - 1 ) );

	for ( size_t i = 0; i < mRowCount - 1; ++i )
	{
		for ( size_t j = 0; j < mColCount - 1; ++j )
		{
			index1 = i * mColCount + j;
			index2 = i * mColCount + j + 1;
			index3 = ( i + 1 ) * mColCount + j;
		
			XMFLOAT3 pos = XMFLOAT3( mHeightmap[ index1 ].x, mHeightmap[ index1 ].y, mHeightmap[ index1 ].z );
			V1 = XMLoadFloat3( &pos );
			pos = XMFLOAT3( mHeightmap[ index2 ].x, mHeightmap[ index2 ].y, mHeightmap[ index2 ].z );
			V2 = XMLoadFloat3( &pos );
			pos = XMFLOAT3( mHeightmap[ index3 ].x, mHeightmap[ index3 ].y, mHeightmap[ index3 ].z );
			V3 = XMLoadFloat3( &pos );
			
			Edge1 = V1 - V3;
			Edge2 = V1 - V2;

			index = i * ( mRowCount - 1 ) + j;
			XMStoreFloat3( &Normals[ index ], XMVector3Cross( Edge1, Edge2 ) );
		}
	}

	int count;
	for ( int i = 0; i < ( int ) mRowCount - 1; ++i )
	{
		for ( int j = 0; j < ( int ) mColCount - 1; ++j )
		{
			Sum = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
			count = 0;
			if ( ( ( i - 1 ) >= 0 ) && ( ( j - 1 ) >= 0 ) )
			{
				index = ( i - 1 ) * ( mRowCount - 1 ) + j - 1;
				Sum += XMLoadFloat3( &Normals[ index ] );
				count++;
			}
			if ( j < ( int ) mColCount - 1 && i - 1 >= 0 )
			{
				index = ( ( i - 1 ) * ( mRowCount - 1 ) ) + j;
				Sum += XMLoadFloat3( &Normals[ index ] );
				count++;
			}
			if ( j - 1 >= 0 && i < ( int ) mRowCount - 1 )
			{
				index = ( i * ( mRowCount - 1 ) ) + ( j - 1 );

				Sum += XMLoadFloat3( &Normals[ index ] );
				count++;
			}
			if ( j < ( int ) mColCount - 1 && i < ( int ) mRowCount - 1 )
			{
				index = ( i * ( mRowCount - 1 ) ) + j;

				Sum += XMLoadFloat3( &Normals[ index ] );
				count++;
			}
			Sum /= ( float ) count;
			Sum = XMVector3Normalize( Sum );
			index = i * mRowCount + j;
			mHeightmap[ index ].nx = XMVectorGetX( Sum );
			mHeightmap[ index ].ny = XMVectorGetY( Sum );
			mHeightmap[ index ].nz = XMVectorGetZ( Sum );
		}
	}
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
		mGrass.get( ) );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}

void CTerrain::RenderMaterials( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, bool bWireframe )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	if ( bWireframe )
		mContext->RSSetState( DX::Wireframe.Get( ) );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	for ( size_t i = 0; i < mMaterials.size( ); ++i )
	{
		if ( mMaterials[ i ].Vertices.size( ) > 0 )
		{
			mContext->IASetVertexBuffers( 0, 1, mMaterials[ i ].VertexBuffer.GetAddressOf( ), &Stride, &Offsets );
			if ( mMaterials[ i ].texture2 == -1 )
			{
				mShader->RenderVertices( mMaterials[ i ].Vertices.size( ), DirectX::XMMatrixIdentity( ), View, Projection,
					mMaterialTextures[ mMaterials[ i ].texture1 ].get( ) );
			}
			else
			{
				mShader->RenderVertices( mMaterials[ i ].Vertices.size( ), DirectX::XMMatrixIdentity( ), View, Projection,
					mMaterialTextures[ mMaterials[ i ].texture1 ].get( ), mMaterialTextures[ mMaterials[ i ].texture2 ].get( ),
					mMaterialTextures[ mMaterials[ i ].alphamap ].get( ) );
			}
		}
	}

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

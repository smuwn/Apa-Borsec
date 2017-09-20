#pragma once

#include "commonincludes.h"
#include "C3DShader.h"

class CTerrain sealed
{
	friend class QuadTree;
private:
	static constexpr const float HeightFactor = 10.0f;
	static constexpr const float TextureRepeat = 10;
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
		SVertex( ) :
			Normal( 0.0f, 1.0f, 0.0f )
		{ };
		SVertex( float x, float y, float z ) :
			Position( x, y, z )
		{ };
	};
	struct SHeightmap
	{
		float x;
		float y;
		float z;
	};
private:

	std::vector<SVertex> mVertices;
	std::vector<DWORD> mIndices;

	std::shared_ptr<C3DShader> mShader;

	std::shared_ptr<CTexture> mTexture;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::vector<SHeightmap> mHeightmap;

	DirectX::XMFLOAT4X4 mWorld;

	BITMAPFILEHEADER mFileHeader;
	BITMAPINFOHEADER mInfoHeader;

	UINT mVertexCount;
	UINT mIndexCount;

	UINT mRowCount;
	UINT mColCount;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr< C3DShader > Shader );
	CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr< C3DShader > Shader, 
		LPSTR Heightmap, LPSTR Normalmap );
	~CTerrain( );
private:
	void InitHeightmap( LPSTR Heightmap );
	void InitHeightmapTerrain( );
	void InitNormals( LPSTR Normalmap );
	void InitTerrain( );
	void InitBuffers( );
private:
	void CopyVertices( void* To );
public:
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, bool bWireframe = false );
public:
	void Identity( );
	void RotateX( float Theta );
	void RotateY( float Theta );
	void RotateZ( float Theta );
	void Translate( float X, float Y, float Z );
	void Scale( float X, float Y, float Z );
};


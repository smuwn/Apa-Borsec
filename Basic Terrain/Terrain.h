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
		DirectX::XMFLOAT4 Texture;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 Binormal;
		DirectX::XMFLOAT4 Color;
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
		float nx;
		float ny;
		float nz;
		float r;
		float g;
		float b;
		int rIndex;
		int gIndex;
		int bIndex;
	};
	struct SMaterial
	{
		int texture1;
		int texture2;
		int alphamap;
		int red;
		int green;
		int blue;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		std::vector<SVertex> Vertices;
	};
private:

	std::vector<SVertex> mVertices;
	std::vector<DWORD> mIndices;

	std::shared_ptr<C3DShader> mShader;

	std::shared_ptr<CTexture> mGrass;
	std::shared_ptr<CTexture> mSlope;
	std::shared_ptr<CTexture> mRock;
	std::shared_ptr<CTexture> mBumpmap;
	std::vector<decltype( mGrass )> mMaterialTextures;
	std::vector<SMaterial> mMaterials;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	std::vector<SHeightmap> mHeightmap;

	DirectX::XMFLOAT4X4 mWorld;

	bool mHasBlendingmap;

	UINT mVertexCount;
	UINT mIndexCount;

	UINT mRowCount;
	UINT mColCount;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr< C3DShader > Shader,
		LPSTR Heightmap, LPSTR Normalmap, LPSTR Colormap,
		LPSTR Materials = "", LPSTR Blendingmap = "" );
	~CTerrain( );
private:
	void InitHeightmap( LPSTR Heightmap, LPSTR Colormap, bool bSmooth = true );
	void InitHeightmapTerrain( );
	void InitNormals( LPSTR Normalmap );
	void InitTerrain( );
	void InitBuffers( );
	void InitMaterials( LPSTR Materials );
	void InitMaterialMap( LPSTR Materialmap );
	void InitMaterialBuffers( );
private:
	void CalculateNormalsFromHeightmap( );
	void Average( int row, int col );
	bool isInBounds( int row, int col );
private:
	void CopyVertices( void* To );
public:
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, bool bWireframe = false );
	void RenderMaterials( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection, bool bWireframe = false );
public:
	void Identity( );
	void RotateX( float Theta );
	void RotateY( float Theta );
	void RotateZ( float Theta );
	void Translate( float X, float Y, float Z );
	void Scale( float X, float Y, float Z );
};


#pragma once

#include "commonincludes.h"
#include "C3DShader.h"

class CTerrain
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		SVertex( ) = default;
		SVertex( float x, float y, float z )
			:Position( x, y, z )
		{ };
	};

private:

	std::vector<SVertex> mVertices;
	std::vector<DWORD> mIndices;

	std::shared_ptr<C3DShader> mShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	UINT mVertexCount;
	UINT mIndexCount;

	UINT mRowCount;
	UINT mColCount;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr< C3DShader > Shader );
	CTerrain( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr< C3DShader > Shader, LPWSTR Heightmap );
	~CTerrain( );
private:
	void InitTerrain( );
	void InitBuffers( );
public:
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
};


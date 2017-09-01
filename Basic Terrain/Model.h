#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

class CModel
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT4 Pos;
		SVertex( DirectX::XMFLOAT3 Pos ) :
			Pos( Pos.x,Pos.y,Pos.z,1.0f )
		{ };
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	UINT mNumVertices;
	UINT mNumIndices;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CModel( ID3D11Device *, ID3D11DeviceContext * );
	~CModel( );
public:
	void Render( );
	inline UINT GetIndexCount( ) { return mNumIndices; };
	inline UINT GetVertexCount( ) { return mNumVertices; };
private:
	void CreateTriangle( );
};


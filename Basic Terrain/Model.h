#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

ALIGN16 class CModel
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 TexCoord;
		SVertex( DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT2 tex ) :
			Pos( Pos ), Normal( Pos ), TexCoord( tex )
		{ };
		SVertex( DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT3 Nor, DirectX::XMFLOAT2 tex ) :
			Pos( Pos.x, Pos.y, Pos.z ),
			Normal( Nor ), TexCoord( tex )
		{ };
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;

	UINT mNumVertices;
	UINT mNumIndices;

	DirectX::XMMATRIX mWorld;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CModel( ID3D11Device *, ID3D11DeviceContext * );
	~CModel( );
public:
	void Render( );
	inline UINT GetIndexCount( ) { return mNumIndices; };
	inline UINT GetVertexCount( ) { return mNumVertices; };
	inline DirectX::XMMATRIX& GetWorld( ) { return mWorld; };
	inline void Identity( ) { mWorld = DirectX::XMMatrixIdentity( ); };
	inline void Scale( float S ) { mWorld *= DirectX::XMMatrixScaling( S, S, S ); };
	inline void Translate( float x, float y, float z ) { mWorld *= DirectX::XMMatrixTranslation( x, y, z ); };
	inline void RotateX( float Theta ) { mWorld *= DirectX::XMMatrixRotationX( Theta ); };
	inline void RotateY( float Theta ) { mWorld *= DirectX::XMMatrixRotationY( Theta ); };
	inline void RotateZ( float Theta ) { mWorld *= DirectX::XMMatrixRotationZ( Theta ); };
private:
	void CreateTriangle( );
public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	}
};


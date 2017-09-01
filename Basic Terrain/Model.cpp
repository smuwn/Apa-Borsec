#include "Model.h"



CModel::CModel( ID3D11Device * device, ID3D11DeviceContext * deviceContext ) :
	mDevice( device ),
	mContext( deviceContext )
{
	try
	{
		CreateTriangle( );
	}
	CATCH;
}

void CModel::CreateTriangle( )
{
	SVertex vertices[ ] =
	{
		SVertex( DirectX::XMFLOAT3( 0.0f, 0.5f, 0.5f ) ),
		SVertex( DirectX::XMFLOAT3( 0.5f,-0.5f, 0.5f ) ),
		SVertex( DirectX::XMFLOAT3( -0.5f,-0.5f, 0.5f ) ),
	};
	mNumVertices = ARRAYSIZE( vertices );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, vertBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, vertBuffData );
	vertBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertBuffDesc.ByteWidth = sizeof( SVertex ) * mNumVertices;
	vertBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	vertBuffData.pSysMem = vertices;
	DX::ThrowIfFailed(
		mDevice->CreateBuffer( &vertBuffDesc, &vertBuffData, &mVertexBuffer )
		);
	DWORD Indices[ ] =
	{
		0, 1, 2
	};
	mNumIndices = ARRAYSIZE( Indices );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, indexBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, indexBuffData );
	indexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.ByteWidth = sizeof( DWORD ) * mNumIndices;
	indexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBuffData.pSysMem = Indices;
	DX::ThrowIfFailed(
		mDevice->CreateBuffer( &indexBuffDesc, &indexBuffData, &mIndexBuffer )
		);
}

void CModel::Render( )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

CModel::~CModel( )
{ }

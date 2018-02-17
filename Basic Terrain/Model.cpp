#include "Model.h"



CModel::CModel( ID3D11Device * device, ID3D11DeviceContext * deviceContext ) :
	mDevice( device ),
	mContext( deviceContext )
{
	try
	{
		CreateTriangle( );
		mWorld = DirectX::XMMatrixIdentity( );
	}
	CATCH;
}

CModel::~CModel( )
{
	mVertexBuffer.Reset( );
	mIndexBuffer.Reset( );
}

void CModel::CreateTriangle( )
{
	SVertex vertices[ ] =
	{
		SVertex( DirectX::XMFLOAT3( -1.0f, -1.0f, -1.0f ), DirectX::XMFLOAT2( 0.0f,1.0f ) ),
		SVertex( DirectX::XMFLOAT3( -1.0f, +1.0f, -1.0f ), DirectX::XMFLOAT2( 0.0f,0.0f ) ),
		SVertex( DirectX::XMFLOAT3( +1.0f, +1.0f, -1.0f ), DirectX::XMFLOAT2( 1.0f,0.0f ) ),
		SVertex( DirectX::XMFLOAT3( +1.0f, -1.0f, -1.0f ), DirectX::XMFLOAT2( 1.0f,1.0f ) ),
		SVertex( DirectX::XMFLOAT3( -1.0f, -1.0f, +1.0f ), DirectX::XMFLOAT2( 0.0f,1.0f ) ),
		SVertex( DirectX::XMFLOAT3( -1.0f, +1.0f, +1.0f ), DirectX::XMFLOAT2( 0.0f,0.0f ) ),
		SVertex( DirectX::XMFLOAT3( +1.0f, +1.0f, +1.0f ), DirectX::XMFLOAT2( 1.0f,0.0f ) ),
		SVertex( DirectX::XMFLOAT3( +1.0f, -1.0f, +1.0f ), DirectX::XMFLOAT2( 1.0f,1.0f ) ),
	};
	mNumVertices = ARRAYSIZE( vertices );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, vertBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, vertBuffData );
	vertBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertBuffDesc.ByteWidth = sizeof( SVertex ) * mNumVertices;
	vertBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	vertBuffData.pSysMem = vertices;
	ThrowIfFailed(
		mDevice->CreateBuffer( &vertBuffDesc, &vertBuffData, &mVertexBuffer )
		);
	DWORD Indices[ ] =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7,
	};
	mNumIndices = ARRAYSIZE( Indices );
	ZeroMemoryAndDeclare( D3D11_BUFFER_DESC, indexBuffDesc );
	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, indexBuffData );
	indexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.ByteWidth = sizeof( DWORD ) * mNumIndices;
	indexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	indexBuffData.pSysMem = Indices;
	ThrowIfFailed(
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

void CModel::Render( DirectX::FXMMATRIX & view, DirectX::FXMMATRIX & projection )
{
	Render( );
	if ( mShader )
	{
		static ModelShader::SPerObjectVS objInfoVS;
		static ModelShader::SPerObjectPS objInfoPS;
		objInfoVS.Projection = DirectX::XMMatrixTranspose( projection );
		objInfoVS.View = DirectX::XMMatrixTranspose( view );
		objInfoVS.World = DirectX::XMMatrixTranspose( mWorld );
		objInfoPS.Color = DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f );
		mContext->RSSetState( DX::NoCulling.Get( ) );
		mShader->Render( GetIndexCount( ), objInfoVS, objInfoPS );
		mContext->RSSetState( DX::DefaultRS.Get( ) );
	}
}

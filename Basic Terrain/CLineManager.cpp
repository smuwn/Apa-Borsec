#include "CLineManager.h"



CLineManager::CLineManager( ID3D11Device * Device, ID3D11DeviceContext * Context,
	std::shared_ptr<LineShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		ShaderHelper::CreateBuffer(
			mDevice, &mVertBuffer, D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * 2 * MAX_LINES, D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);
	}
	CATCH;
}


CLineManager::~CLineManager( )
{
	mVertBuffer.Reset( );
}


void CLineManager::Begin( )
{
	static D3D11_MAPPED_SUBRESOURCE Mapped;
	ThrowIfFailed(
		mContext->Map( mVertBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped )
	);
	mAddress = Mapped.pData;
}

void CLineManager::End( )
{
	mContext->Unmap( mVertBuffer.Get( ), 0 );
}

void CLineManager::Line( DirectX::XMFLOAT3 const & From, DirectX::XMFLOAT3 const & To )
{
	SVertex* VertexAddress = ( SVertex* ) mAddress;
	VertexAddress[ mVertexToAdd ].Position.x = From.x;
	VertexAddress[ mVertexToAdd ].Position.y = From.y;
	VertexAddress[ mVertexToAdd ].Position.z = From.z;
	VertexAddress[ mVertexToAdd + 1 ].Position.x = To.x;
	VertexAddress[ mVertexToAdd + 1 ].Position.y = To.y;
	VertexAddress[ mVertexToAdd + 1 ].Position.z = To.z;
	mVertexToAdd += 2;
}

void CLineManager::Render( DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offset = 0;
	mContext->IASetVertexBuffers( 0, 1, mVertBuffer.GetAddressOf( ), &Stride, &Offset );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	mContext->RSSetState( DX::NoCulling.Get( ) );
	mShader->Render( mVertexToAdd, View, Projection );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
	mVertexToAdd = 0;
}

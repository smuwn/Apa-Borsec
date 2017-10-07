#include "Skybox.h"


Skybox::Skybox( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{	
		GeometryGenerator::MeshData Sphere;
		GeometryGenerator::CreateSphere( 1.0f, 10, 10, mSphere );

		ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof( DWORD ) * mSphere.Indices.size( ), 0, &mSphere.Indices[ 0 ] );

		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mSphere.Vertices.size( ), 0, &mSphere.Vertices[ 0 ] );

		SkyShader::SPSColor Color;
		Color.CenterColor = DirectX::XMFLOAT4( 0.81f, 0.38f, 0.66f, 1.0f );
		Color.ApexColor = DirectX::XMFLOAT4( 0.0f, 0.15f, 0.66f, 1.0f );
		mShader->SetColors( Color );

	}
	CATCH;
}


Skybox::~Skybox( )
{
	mVertexBuffer.Reset( );
	mShader.reset( );
}

void Skybox::Update( DirectX::XMFLOAT3 const& CamPos )
{
	mWorld = DirectX::XMMatrixTranslation( CamPos.x, CamPos.y, CamPos.z );
}

void Skybox::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	mContext->RSSetState( DX::FrontCulling.Get( ) );
	mContext->OMSetDepthStencilState( DX::DSLessEqual.Get( ), 0 );
	mShader->Render( mSphere.Indices.size( ), mWorld, View, Projection );
	mContext->OMSetDepthStencilState( 0, 0 );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
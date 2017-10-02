#include "Skybox.h"



Skybox::Skybox( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		// Dear future me,
		// Please, improve the model of the skybox
		// I'll give you a handjob
		//						I remain
		//					your sincere friend
		//						Past me
		SVertex vertices[ ] =
		{
			SVertex( -1.0f, -1.0f, -1.0f ),
			SVertex( -1.0f, +1.0f, -1.0f ),
			SVertex( +1.0f, +1.0f, -1.0f ),
			SVertex( +1.0f, -1.0f, -1.0f ),
			SVertex( -1.0f, -1.0f, +1.0f ),
			SVertex( -1.0f, +1.0f, +1.0f ),
			SVertex( +1.0f, +1.0f, +1.0f ),
			SVertex( +1.0f, -1.0f, +1.0f ),
		};
		mVertexCount = ARRAYSIZE( vertices );

		DWORD indices[ ] =
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
		mIndexCount = ARRAYSIZE( indices );

		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mVertexCount, 0, vertices );

		ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof( DWORD ) * mIndexCount, 0, indices );

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
	mIndexBuffer.Reset( );
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

	mContext->RSSetState( DX::NoCulling.Get( ) );
	mContext->OMSetDepthStencilState( DX::DSLessEqual.Get( ), 0 );
	mShader->Render( mIndexCount, mWorld, View, Projection );
	mContext->OMSetDepthStencilState( 0, 0 );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
#include "Skybox.h"


Skybox::Skybox( ID3D11Device * Device, ID3D11DeviceContext * Context, std::shared_ptr<SkyShader> Shader ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
#if !(DEBUG || _DEBUG)
		LoadModel( L"Data/Sphere.txt" );
		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mVertexCount, 0, &mVertices[ 0 ] );
#else
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

		ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
			sizeof( DWORD ) * mIndexCount, 0, &indices[ 0 ] );

		ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * mVertexCount, 0, &vertices );
#endif

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

void Skybox::LoadModel( LPWSTR lpPath )
{
	std::wifstream ifModel( lpPath );

	if ( !ifModel.is_open( ) )
		throw std::exception( "Couldn't open model file" );

	wchar_t ch = 0;
	while ( ch != ':' )
	{
		ifModel.get( ch );
	}
	ifModel >> mVertexCount;

	ifModel.get( ch );
	while ( ch != ':' )
	{
		ifModel.get( ch );
	}

	for ( int i = 0; i < ( int ) mVertexCount; ++i )
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		ifModel >> x >> y >> z;
		
		ifModel >> u >> v >> nx >> ny >> nz;

		mVertices.emplace_back( x, y, z );
	}

	ifModel.close( );
}


void Skybox::Update( DirectX::XMFLOAT3 const& CamPos )
{
	mWorld = DirectX::XMMatrixTranslation( CamPos.x, CamPos.y, CamPos.z );
}

void Skybox::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static UINT Stride = sizeof( SVertex );
	static UINT Offsets = 0;
#if DEBUG || _DEBUG
	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
#endif
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offsets );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

//#if DEBUG || _DEBUG
	mContext->RSSetState( DX::NoCulling.Get( ) );
//#endif
	mContext->OMSetDepthStencilState( DX::DSLessEqual.Get( ), 0 );
#if DEBUG || _DEBUG
	mShader->Render( mIndexCount, mWorld, View, Projection );
#else
	mShader->RenderVertices( mVertexCount, mWorld, View, Projection );
#endif
	mContext->OMSetDepthStencilState( 0, 0 );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}
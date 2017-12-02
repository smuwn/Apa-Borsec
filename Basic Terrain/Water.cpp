#include "Water.h"



CWater::CWater( ID3D11Device * Device, ID3D11DeviceContext * Context,
	std::shared_ptr<CWaterShader> Shader, float radius, UINT numQuads,
	UINT repeatTexture ) :
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		InitializeBuffers( radius, numQuads, repeatTexture );
	}
	CATCH;
}


CWater::~CWater( )
{
	mShader.reset( );

	mVertexBuffer.Reset( );
	mIndexBuffer.Reset( );
}

void CWater::SetTextures( ID3D11ShaderResourceView * Reflection, ID3D11ShaderResourceView * Refraction )
{
	mReflection.reset( );
	mReflection = std::make_unique<CTexture>( );
	mReflection->SetTexture( Reflection );
	mRefraction.reset( );
	mRefraction = std::make_unique<CTexture>( );
	mRefraction->SetTexture( Refraction );
}

void CWater::Update( float frameTime, CCamera * camera )
{
	auto CamPos = camera->GetCamPos( );
	auto CamRot = camera->GetCamRotation( );
	mWorld = DirectX::XMMatrixRotationY( CamRot.y ) *
		DirectX::XMMatrixTranslation( CamPos.x, 0.0f, CamPos.z );
}

void CWater::Render( DirectX::FXMMATRIX & view, DirectX::FXMMATRIX & projection, DirectX::FXMMATRIX & reflectView )
{
	static const UINT Stride = sizeof( SVertex );
	static const UINT Offset = 0;

	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	mContext->IASetIndexBuffer( mIndexBuffer.Get( ), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0 );
	mContext->IASetVertexBuffers( 0, 1, mVertexBuffer.GetAddressOf( ), &Stride, &Offset );

	mContext->RSSetState( DX::NoCulling.Get( ) );
	mShader->Render( mIndexCount, mWorld, view, projection, reflectView, mReflection.get( ), mRefraction.get( ) );
	mContext->RSSetState( DX::DefaultRS.Get( ) );
}

void CWater::InitializeBuffers( float radius, UINT numQuads, UINT repeatTexture )
{
	float dx = ( 2 * radius ) / ( float ) numQuads;
	float dz = ( 2 * radius ) / ( float ) numQuads;

	float du = ( float ) repeatTexture / ( float ) numQuads;
	float dv = ( float ) repeatTexture / ( float ) numQuads;

	float dtu = 1.0f / ( float ) numQuads;
	float dtv = 1.0f / ( float ) numQuads;

	mVertices.resize( ( numQuads ) * ( numQuads ) );
	mVertexCount = mVertices.size( );
	for ( size_t i = 0; i < numQuads; ++i )
	{
		for ( size_t j = 0; j < numQuads; ++j )
		{
			int index = i * numQuads + j;
			mVertices[ index ].Position.x = -radius + ( float ) j * dx;
			mVertices[ index ].Position.y = 0.0f;
			mVertices[ index ].Position.z = -10.0f + ( float ) i * dz;
		}
	}

	mIndices.resize( ( numQuads - 1 ) * ( numQuads - 1 ) * 6 );
	mIndexCount = mIndices.size( );
	int index = 0;
	float u = 0.0f;
	float v = 0.0f;
	float tu = 0.0f;
	float tv = 0.0f;
	for ( size_t i = 0; i < numQuads - 1; ++i )
	{
		for ( size_t j = 0; j < numQuads - 1; ++j )
		{
			// Bottom left
			mIndices[ index + 0 ] = ( i + 1 ) * numQuads + j;
			mVertices[ ( i + 1 ) * numQuads + j ].Texture = DirectX::XMFLOAT2(  tu, tv + dv );

			// Bottom right
			mIndices[ index + 1 ] = ( i + 1 ) * numQuads + j + 1;
			mVertices[ ( i + 1 ) * numQuads + j + 1 ].Texture = DirectX::XMFLOAT2( tu + du, tv + dv );

			// Top left
			mIndices[ index + 2 ] = i * numQuads + j;
			mVertices[ i * numQuads + j ].Texture = DirectX::XMFLOAT2( tu, tv );

			mIndices[ index + 3 ] = ( i + 1 ) * numQuads + j + 1; // Bottom right

			// Top right
			mIndices[ index + 4 ] = i * numQuads + j + 1;
			mVertices[ i * numQuads + j + 1 ].Texture = DirectX::XMFLOAT2(  tu + du, tv );

			mIndices[ index + 5 ] = i * numQuads + j; // Top left

			index += 6;
			tu += du;
		}
		tu = 0.0f;
		tv += dv;
	}

	ShaderHelper::CreateBuffer( mDevice, &mVertexBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
		sizeof( SVertex ) * mVertexCount, 0, &mVertices[ 0 ] );

	ShaderHelper::CreateBuffer( mDevice, &mIndexBuffer,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER,
		sizeof( DWORD ) * mIndexCount, 0, &mIndices[ 0 ] );
}


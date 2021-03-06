#include "SkyShader.h"



SkyShader::SkyShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;
		ShaderHelper::CreateShaderFromFile( L"Shaders/SkyVertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/SkyPixelShader.cso", "ps_4_0",
			mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );
		D3D11_INPUT_ELEMENT_DESC layout[ 1 ];
		layout[ 0 ].AlignedByteOffset = 0;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		ThrowIfFailed(
			mDevice->CreateInputLayout( layout, ARRAYSIZE( layout ),
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mLayout )
			);

		ShaderHelper::CreateBuffer( mDevice, &mVSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SVSPerObject ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
		ShaderHelper::CreateBuffer( mDevice, &mPSColorBuffer,
			D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SPSColor ), 0 );
	}
	CATCH;
}


SkyShader::~SkyShader( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
		mBlobs[ i ].Reset( );

	mVertexShader.Reset( );
	mPixelShader.Reset( );
	mLayout.Reset( );
	mVSPerObjectBuffer.Reset( );
	mPSColorBuffer.Reset( );
}



void SkyShader::Render( UINT IndexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	mContext->IASetInputLayout( mLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), 0, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), 0, 0 );

	mContext->PSSetConstantBuffers( 0, 1, mPSColorBuffer.GetAddressOf( ) );

	mContext->Map( mVSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SVSPerObject* ) MappedSubresource.pData )->WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Unmap( mVSPerObjectBuffer.Get( ),0 );
	mContext->VSSetConstantBuffers( 0, 1, mVSPerObjectBuffer.GetAddressOf( ) );

	mContext->DrawIndexed( IndexCount, 0, 0 );
}

void SkyShader::RenderVertices( UINT VertexCount, DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	mContext->IASetInputLayout( mLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), 0, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), 0, 0 );

	mContext->PSSetConstantBuffers( 0, 1, mPSColorBuffer.GetAddressOf( ) );

	mContext->Map( mVSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SVSPerObject* ) MappedSubresource.pData )->WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Unmap( mVSPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mVSPerObjectBuffer.GetAddressOf( ) );

	mContext->Draw( VertexCount, 0 );
}


void SkyShader::SetColors( SkyShader::SPSColor const& Color )
{
	mContext->UpdateSubresource( mPSColorBuffer.Get( ), 0, nullptr, &Color, 0, 0 );
}
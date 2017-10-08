#include "SkyPlaneShader.h"



SkyPlaneShader::SkyPlaneShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;
		ShaderHelper::CreateShaderFromFile( L"Shaders/SkyPlaneVertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/SkyPlanePixelShader.cso", "ps_4_0",
			mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );
		D3D11_INPUT_ELEMENT_DESC layout[2];
		layout[ 0 ].AlignedByteOffset = 0;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 1 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 1 ].InputSlot = 0;
		layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 1 ].InstanceDataStepRate = 0;
		layout[ 1 ].SemanticIndex = 0;
		layout[ 1 ].SemanticName = "TEXCOORD";
		int numElements = ARRAYSIZE( layout );
		DX::ThrowIfFailed(
			mDevice->CreateInputLayout( layout, numElements,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mLayout )
			);
		ShaderHelper::CreateBuffer( mDevice, &mVSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SVSPerObject ), D3D11_CPU_ACCESS_WRITE );
		ShaderHelper::CreateBuffer( mDevice, &mPSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SPSPerObject ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );

		ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
		sampDesc.AddressU =
			sampDesc.AddressV =
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_ALWAYS;
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MaxLOD = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MipLODBias = 3;
		DX::ThrowIfFailed(
			mDevice->CreateSamplerState( &sampDesc, &mWrapSampler )
			);
	}
	CATCH;
}


SkyPlaneShader::~SkyPlaneShader( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
		mBlobs[ i ].Reset( );
	mVertexShader.Reset( );
	mPixelShader.Reset( );

	mVSPerObjectBuffer.Reset( );
	mPSPerObjectBuffer.Reset( );
	mWrapSampler.Reset( );
}

void SkyPlaneShader::Render( UINT IndexCount, float brightness,
	DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection,
	STextureInfo const & FirstTexture, STextureInfo const & SecondTexture )
{
	// Replace the old toys
	mContext->IASetInputLayout( mLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );


	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	DirectX::XMMATRIX WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Map( mVSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SVSPerObject* ) MappedSubresource.pData )->WVP = WVP;

	mContext->Unmap( mVSPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mVSPerObjectBuffer.GetAddressOf( ) );

	mContext->PSSetSamplers( 0, 1, mWrapSampler.GetAddressOf( ) );

	if ( FirstTexture.Texture /*!= nullptr*/ && SecondTexture.Texture /*!= nullptr*/ )
	{
		ID3D11ShaderResourceView * SRV = FirstTexture.Texture->GetTexture( );
		mContext->PSSetShaderResources( 0, 1, &SRV );
		SRV = SecondTexture.Texture->GetTexture( );
		mContext->PSSetShaderResources( 1, 1, &SRV );

		mContext->Map( mPSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
		
		( ( SPSPerObject* ) MappedSubresource.pData )->FirstTextureOffset = FirstTexture.Offset;
		( ( SPSPerObject* ) MappedSubresource.pData )->SecondTextureOffset = SecondTexture.Offset;
		( ( SPSPerObject* ) MappedSubresource.pData )->brightness = brightness;

		mContext->Unmap( mPSPerObjectBuffer.Get( ), 0 );
		mContext->PSSetConstantBuffers( 0, 1, mPSPerObjectBuffer.GetAddressOf( ) );

		mContext->DrawIndexed( IndexCount, 0, 0 );
	}
}

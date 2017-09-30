#include "C3DShader.h"



C3DShader::C3DShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;
		ShaderHelper::CreateShaderFromFile( L"Shaders/3DVertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/3DPixelShader.cso", "ps_4_0",
			mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );
		D3D11_INPUT_ELEMENT_DESC layout[ 4 ];
		layout[ 0 ].AlignedByteOffset = 0;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 1 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		layout[ 1 ].InputSlot = 0;
		layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 1 ].InstanceDataStepRate = 0;
		layout[ 1 ].SemanticIndex = 0;
		layout[ 1 ].SemanticName = "TEXCOORD";
		layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 2 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 2 ].InputSlot = 0;
		layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 2 ].InstanceDataStepRate = 0;
		layout[ 2 ].SemanticIndex = 0;
		layout[ 2 ].SemanticName = "NORMAL";
		layout[ 3 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 3 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[ 3 ].InputSlot = 0;
		layout[ 3 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 3 ].InstanceDataStepRate = 0;
		layout[ 3 ].SemanticIndex = 0;
		layout[ 3 ].SemanticName = "COLOR";
		UINT layoutCount = ARRAYSIZE( layout );
		DX::ThrowIfFailed(
			mDevice->CreateInputLayout( layout, layoutCount,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mInputLayout )
			);
		ShaderHelper::CreateBuffer( mDevice, &mPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SPerObject ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
		ShaderHelper::CreateBuffer( mDevice, &mLightBuffer,
			D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SLight ), 0 );
		ShaderHelper::CreateBuffer( mDevice, &mTextureBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( STexture ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
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


C3DShader::~C3DShader( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
		mBlobs[ i ].Reset( );
	mVertexShader.Reset( );
	mInputLayout.Reset( );
	mPixelShader.Reset( );
	mPerObjectBuffer.Reset( );
	mTextureBuffer.Reset( );
	mWrapSampler.Reset( );
}

void C3DShader::Render( UINT IndexCount, DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection,
	CTexture * texture )
{
	// Replace the old toys
	mContext->IASetInputLayout( mInputLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );

	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	DirectX::XMMATRIX WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Map( mPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SPerObject* ) MappedSubresource.pData )->WVP = WVP;
	( ( SPerObject* ) MappedSubresource.pData )->World = DirectX::XMMatrixTranspose( World );

	mContext->Unmap( mPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mPerObjectBuffer.GetAddressOf( ) );
	
	mContext->PSSetSamplers( 0, 1, mWrapSampler.GetAddressOf( ) );

	mContext->PSSetConstantBuffers( 0, 1, mLightBuffer.GetAddressOf( ) );
	ID3D11ShaderResourceView * SRV = texture->GetTexture( );
	mContext->PSSetShaderResources( 0, 1, &SRV );

	mContext->DrawIndexed( IndexCount, 0, 0 );
}

void C3DShader::RenderVertices( UINT IndexCount, DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection,
	CTexture * texture, CTexture * texture2, CTexture * Alpha )
{
	// Replace the old toys
	mContext->IASetInputLayout( mInputLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );

	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	DirectX::XMMATRIX WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Map( mPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SPerObject* ) MappedSubresource.pData )->WVP = WVP;
	( ( SPerObject* ) MappedSubresource.pData )->World = DirectX::XMMatrixTranspose( World );

	mContext->Unmap( mPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mPerObjectBuffer.GetAddressOf( ) );

	bool bUseAlpha = false;
	if ( texture2 != nullptr && Alpha != nullptr )
	{
		ID3D11ShaderResourceView *SRV = texture2->GetTexture( );
		mContext->PSSetShaderResources( 1, 1, &SRV );
		SRV = Alpha->GetTexture( );
		mContext->PSSetShaderResources( 2, 1, &SRV );
	}

	mContext->Map( mTextureBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( STexture* ) MappedSubresource.pData )->HasAlpha = bUseAlpha;
	mContext->Unmap( mTextureBuffer.Get( ), 0 );
	mContext->PSSetConstantBuffers( 1, 1, mTextureBuffer.GetAddressOf( ) );

	mContext->PSSetSamplers( 0, 1, mWrapSampler.GetAddressOf( ) );

	mContext->PSSetConstantBuffers( 0, 1, mLightBuffer.GetAddressOf( ) );
	ID3D11ShaderResourceView * SRV = texture->GetTexture( );
	mContext->PSSetShaderResources( 0, 1, &SRV );

	mContext->Draw( IndexCount, 0 );
}


void C3DShader::SetLight( C3DShader::SLight const& Light )
{
	mContext->UpdateSubresource( mLightBuffer.Get( ), 0, nullptr, &Light, 0, 0 );
}
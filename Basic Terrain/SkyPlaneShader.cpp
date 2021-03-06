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
		ThrowIfFailed(
			mDevice->CreateInputLayout( layout, numElements,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mLayout )
			);
		ShaderHelper::CreateBuffer( mDevice, &mVSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SVSPerObject ), D3D11_CPU_ACCESS_WRITE );
		ShaderHelper::CreateBuffer( mDevice, &mPSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( STextureInfo ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );

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
}

void SkyPlaneShader::Render( UINT IndexCount, SkyPlaneShader::STextureInfo const& TextureInfo,
	DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection,
	CTexture const* CloudTexture, CTexture const* PerturbTexture )
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

	mContext->PSSetSamplers( 0, 1, DX::AnisotropicWrapSampler.GetAddressOf( ) );
	if ( CloudTexture != nullptr && PerturbTexture != nullptr )
	{
		ID3D11ShaderResourceView * SRV = CloudTexture->GetTexture( );
		mContext->PSSetShaderResources( 0, 1, &SRV );
		SRV = PerturbTexture->GetTexture( );
		mContext->PSSetShaderResources( 1, 1, &SRV );

		mContext->Map( mPSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

		( ( STextureInfo* ) MappedSubresource.pData )->brightness = TextureInfo.brightness;
		( ( STextureInfo* ) MappedSubresource.pData )->scale = TextureInfo.scale;
		( ( STextureInfo* ) MappedSubresource.pData )->translation = TextureInfo.translation;

		mContext->Unmap( mPSPerObjectBuffer.Get( ),0 );
		mContext->PSSetConstantBuffers( 0, 1, mPSPerObjectBuffer.GetAddressOf( ) );

		mContext->DrawIndexed( IndexCount, 0, 0 );
	}

}

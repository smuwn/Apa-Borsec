#include "WaterShader.h"



CWaterShader::CWaterShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;

		ShaderHelper::CreateShaderFromFile( L"Shaders/WaterVertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/WaterPixelShader.cso", "ps_4_0",
			mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );

		D3D11_INPUT_ELEMENT_DESC layout[ 2 ];
		layout[ 0 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		layout[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layout[ 1 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[ 1 ].InputSlot = 0;
		layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 1 ].InstanceDataStepRate = 0;
		layout[ 1 ].SemanticIndex = 0;
		layout[ 1 ].SemanticName = "TEXCOORD";
		UINT numArray = ARRAYSIZE( layout );

		DX::ThrowIfFailed(
			mDevice->CreateInputLayout( layout, numArray,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ),
				&mLayout )
			);

		ShaderHelper::CreateBuffer( mDevice, &mVSPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SVSPerObject ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );

		ZeroMemoryAndDeclare( D3D11_SAMPLER_DESC, sampDesc );
		sampDesc.AddressU =
			sampDesc.AddressV =
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MaxLOD = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MipLODBias = 0;
		DX::ThrowIfFailed(
			mDevice->CreateSamplerState( &sampDesc, &mWrapSampler )
			);
	}
	CATCH;
}


CWaterShader::~CWaterShader( )
{
	for ( auto & iter : mBlobs )
	{
		iter.Reset( );
	}

	mVertexShader.Reset( );
	mPixelShader.Reset( );
	mLayout.Reset( );

	mVSPerObjectBuffer.Reset( );
}

void CWaterShader::Render( UINT indexCount,
	DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View,
	DirectX::FXMMATRIX & Projection, DirectX::FXMMATRIX & ReflectView,
	CTexture * Reflection, CTexture * Refraction )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );
	mContext->IASetInputLayout( mLayout.Get( ) );

	mContext->PSSetSamplers( 0, 1, mWrapSampler.GetAddressOf( ) );
	ID3D11ShaderResourceView * SRV[ 2 ] = { Reflection->GetTexture( ),Refraction->GetTexture( ) };
	mContext->PSSetShaderResources( 0, 2, SRV );
	
	mContext->Map( mVSPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SVSPerObject* ) MappedSubresource.pData )->World = DirectX::XMMatrixTranspose( World );
	( ( SVSPerObject* ) MappedSubresource.pData )->View = DirectX::XMMatrixTranspose( View );
	( ( SVSPerObject* ) MappedSubresource.pData )->Projection = DirectX::XMMatrixTranspose( Projection );
	( ( SVSPerObject* ) MappedSubresource.pData )->ReflectView = DirectX::XMMatrixTranspose( ReflectView );

	mContext->Unmap( mVSPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mVSPerObjectBuffer.GetAddressOf( ) );

	mContext->DrawIndexed( indexCount, 0, 0 );
}



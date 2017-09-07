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
		D3D11_INPUT_ELEMENT_DESC layout[ 1 ];
		layout[ 0 ].AlignedByteOffset = 0;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		UINT layoutCount = ARRAYSIZE( layout );
		DX::ThrowIfFailed(
			mDevice->CreateInputLayout( layout, layoutCount,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mInputLayout )
			);
		ShaderHelper::CreateBuffer( mDevice, &mPerObjectBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SPerObject ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
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
}

void C3DShader::Render( UINT IndexCount, DirectX::FXMMATRIX & World, DirectX::FXMMATRIX & View, DirectX::FXMMATRIX & Projection )
{
	// Old toys addresses
	ID3D11VertexShader * oldVS = nullptr;
	ID3D11PixelShader * oldPS = nullptr;
	ID3D11InputLayout * oldLayout = nullptr;
	ID3D11SamplerState * oldSampler = nullptr;
	ID3D11Buffer * oldBuffer = nullptr;
	ID3D11ShaderResourceView * oldSRV = nullptr;

	// Get the old toys
	mContext->IAGetInputLayout( &oldLayout );
	mContext->VSGetShader( &oldVS, nullptr, nullptr );
	mContext->PSGetShader( &oldPS, nullptr, nullptr );
	mContext->PSGetSamplers( 0, 1, &oldSampler );
	mContext->VSGetConstantBuffers( 0, 1, &oldBuffer );
	mContext->PSGetShaderResources( 0, 1, &oldSRV );

	// Replace the old toys
	mContext->IASetInputLayout( mInputLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );

	D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	DirectX::XMMATRIX WVP = DirectX::XMMatrixTranspose( World * View * Projection );

	mContext->Map( mPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );

	( ( SPerObject* ) MappedSubresource.pData )->WVP = WVP;

	mContext->Unmap( mPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mPerObjectBuffer.GetAddressOf( ) );

	mContext->DrawIndexed( IndexCount, 0, 0 );

	// Place back the old toys
	mContext->IASetInputLayout( oldLayout );
	mContext->VSSetShader( oldVS, nullptr, 0 );
	mContext->PSSetShader( oldPS, nullptr, 0 );
	mContext->PSSetSamplers( 0, 1, &oldSampler );
	mContext->VSSetConstantBuffers( 0, 1, &oldBuffer );
	mContext->PSSetShaderResources( 0, 1, &oldSRV );
}



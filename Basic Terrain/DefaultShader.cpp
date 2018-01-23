#include "DefaultShader.h"



CDefaultShader::CDefaultShader( ID3D11Device * device, ID3D11DeviceContext * context ) :
	mDevice( device ),
	mContext( context )
{
	try
	{
		ID3D11VertexShader ** vertexShader = &mVertexShader;
		ID3D11PixelShader ** pixelShader = &mPixelShader;
		ShaderHelper::CreateShaderFromFile( L"Shaders/VertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( vertexShader ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/PixelShader.cso", "ps_4_0",
			mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( pixelShader ) );
		D3D11_INPUT_ELEMENT_DESC layout[ 1 ];
		layout[ 0 ].AlignedByteOffset = 0;
		layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		layout[ 0 ].InputSlot = 0;
		layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
		layout[ 0 ].InstanceDataStepRate = 0;
		layout[ 0 ].SemanticIndex = 0;
		layout[ 0 ].SemanticName = "POSITION";
		UINT layoutCount = ARRAYSIZE( layout );
		ThrowIfFailed(
			mDevice->CreateInputLayout(
				layout, layoutCount, mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mInputLayout
				)
			);
	}
	CATCH;
}

CDefaultShader::~CDefaultShader( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
		mBlobs[ i ].Reset( );
	mVertexShader.Reset( );
	mPixelShader.Reset( );
	mInputLayout.Reset( );
}

void CDefaultShader::Render( UINT indexCount )
{
	// Old toys addresses
	ID3D11VertexShader * oldVS;
	ID3D11PixelShader * oldPS;
	ID3D11InputLayout * oldLayout;

	// Get the old toys
	mContext->IAGetInputLayout( &oldLayout );
	mContext->VSGetShader( &oldVS, nullptr, nullptr );
	mContext->PSGetShader( &oldPS, nullptr, nullptr );

	// Replace the old toys with new toys
	mContext->IASetInputLayout( mInputLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );
	mContext->DrawIndexed( indexCount, 0, 0 );

	// Place back the old toys
	mContext->IASetInputLayout( oldLayout );
	mContext->VSSetShader( oldVS, nullptr, 0 );
	mContext->PSSetShader( oldPS, nullptr, 0 );
}
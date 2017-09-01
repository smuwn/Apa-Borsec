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
		DX::ThrowIfFailed(
			mDevice->CreateInputLayout(
				layout, layoutCount, mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mInputLayout
				)
			);
	}
	CATCH;
}

void CDefaultShader::Render( UINT indexCount )
{
	ID3D11VertexShader * oldVS;
	ID3D11PixelShader * oldPS;
	ID3D11InputLayout * oldLayout;

	mContext->IAGetInputLayout( &oldLayout );
	mContext->VSGetShader( &oldVS, nullptr, nullptr );
	mContext->PSGetShader( &oldPS, nullptr, nullptr );

	mContext->IASetInputLayout( mInputLayout.Get( ) );
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );
	mContext->DrawIndexed( indexCount, 0, 0 );

	mContext->IASetInputLayout( oldLayout );
	mContext->VSSetShader( oldVS, nullptr, 0 );
	mContext->PSSetShader( oldPS, nullptr, 0 );
}

CDefaultShader::~CDefaultShader( )
{ }

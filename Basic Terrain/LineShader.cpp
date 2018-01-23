#include "LineShader.h"



LineShader::LineShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;
		ShaderHelper::CreateShaderFromFile( L"Shaders/LineVertexShader.cso", "vs_4_0",
			mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/LinePixelShader.cso", "ps_4_0",
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
		ThrowIfFailed(
			mDevice->CreateInputLayout( layout, layoutCount,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mInputLayout )
		);
		ShaderHelper::CreateBuffer(
			mDevice, &mPerObjectBuffer, D3D11_USAGE::D3D11_USAGE_DYNAMIC,
			D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, sizeof( SPerObject ),
			D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
		);
	}
	CATCH;
}


LineShader::~LineShader( )
{
	for ( size_t i = 0; i < mBlobs.size( ); ++i )
		mBlobs[ i ].Reset( );
	mVertexShader.Reset( );
	mPixelShader.Reset( );
	mInputLayout.Reset( );
	mPerObjectBuffer.Reset( );
}

void LineShader::Render( int VertexCount, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
{
	mContext->VSSetShader( mVertexShader.Get( ), 0, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), 0, 0 );
	mContext->IASetInputLayout( mInputLayout.Get( ) );

	static DirectX::XMMATRIX WVP;
	static D3D11_MAPPED_SUBRESOURCE Mapped;
	WVP = View * Projection;
	ThrowIfFailed(
		mContext->Map( mPerObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &Mapped )
	);
	( ( SPerObject* ) Mapped.pData )->WVP = DirectX::XMMatrixTranspose( WVP );
	mContext->Unmap( mPerObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mPerObjectBuffer.GetAddressOf( ) );

	mContext->Draw( VertexCount, 0 );

}
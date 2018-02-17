#include "ShadowMapShader.h"



ShadowMapShader::ShadowMapShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		InitializeShaders( );
		InitializeBuffers( );
	}
	CATCH;
}


ShadowMapShader::~ShadowMapShader( )
{
}

void ShadowMapShader::InitializeShaders( )
{
	ID3D11VertexShader ** VS = mVertexShader.GetAddressOf( );
	ID3D11PixelShader ** PS = mPixelShader.GetAddressOf( );

	ShaderHelper::CreateShaderFromFile( L"Shaders/ShadowMapVertexShader.cso", "vs_4_0",
		mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );

	ShaderHelper::CreateShaderFromFile( L"Shaders/ShadowMapPixelShader.cso", "ps_4_0",
		mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );

	D3D11_INPUT_ELEMENT_DESC layout[ 2 ];
	layout[ 0 ].AlignedByteOffset = 0;
	layout[ 0 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
	layout[ 0 ].InputSlot = 0;
	layout[ 0 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 0 ].InstanceDataStepRate = 0;
	layout[ 0 ].SemanticIndex = 0;
	layout[ 0 ].SemanticName = "POSITION";
	layout[ 1 ].AlignedByteOffset = 0;
	layout[ 1 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layout[ 1 ].InputSlot = 0;
	layout[ 1 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 1 ].InstanceDataStepRate = 0;
	layout[ 1 ].SemanticIndex = 0;
	layout[ 1 ].SemanticName = "TEXCOORD";
	UINT layoutSize = ARRAYSIZE( layout );

	ThrowIfFailed( mDevice->CreateInputLayout( layout, layoutSize,
		mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mLayout ) );
}

void ShadowMapShader::InitializeBuffers( )
{
	ShaderHelper::CreateBuffer( mDevice, &mObjectBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SObjectBuffer ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );

	ShaderHelper::CreateBuffer( mDevice, &mMaterialBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SMaterialBuffer ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
}

void ShadowMapShader::PrepareForRendering( DirectX::FXMMATRIX & WVP,
	BOOL bHasTexture, ID3D11ShaderResourceView * srv )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );
	mContext->IASetInputLayout( mLayout.Get( ) );

	if ( bHasTexture )
		mContext->PSSetShaderResources( 0, 1, &srv );
	else
	{
		ID3D11ShaderResourceView * nullSRV[ ] = { nullptr };
		mContext->PSSetShaderResources( 0, 1, nullSRV );
	}

	mContext->Map( mObjectBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SObjectBuffer* ) MappedSubresource.pData )->WVP = DirectX::XMMatrixTranspose( WVP );
	mContext->Unmap( mObjectBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mObjectBuffer.GetAddressOf( ) );

	mContext->Map( mMaterialBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	( ( SMaterialBuffer* ) MappedSubresource.pData )->HasTexture = bHasTexture;
	mContext->Unmap( mMaterialBuffer.Get( ), 0 );
	mContext->PSSetConstantBuffers( 0, 1, mMaterialBuffer.GetAddressOf( ) );

	mContext->PSSetSamplers( 0, 1, DX::LinearWrapSampler.GetAddressOf( ) );
}

void ShadowMapShader::RenderIndices( UINT indexCount, DirectX::FXMMATRIX & WVP,
	BOOL bHasTexture, ID3D11ShaderResourceView * srv )
{
	PrepareForRendering( WVP, bHasTexture, srv );

	mContext->DrawIndexed( indexCount, 0, 0 );
}

void ShadowMapShader::RenderVertices( UINT vertexCount, DirectX::FXMMATRIX& WVP,
	BOOL bHasTexture, ID3D11ShaderResourceView * srv )
{
	PrepareForRendering( WVP, bHasTexture, srv );

	mContext->Draw( vertexCount, 0 );
}

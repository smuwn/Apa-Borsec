#include "ModelShader.h"



ModelShader::ModelShader( ID3D11Device * Device, ID3D11DeviceContext * Context ) :
	mDevice( Device ),
	mContext( Context )
{
	try
	{
		CreateShaders( );
		CreateBuffers( );
	}
	CATCH;
}


ModelShader::~ModelShader( )
{
}

void ModelShader::CreateShaders( )
{
	ID3D11VertexShader ** VS = &mVertexShader;
	ID3D11PixelShader ** PS = &mPixelShader;

	ShaderHelper::CreateShaderFromFile( L"Shaders/ModelVertexShader.cso", "vs_4_0",
		mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
	ShaderHelper::CreateShaderFromFile( L"Shaders/ModelPixelShader.cso", "ps_4_0",
		mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );

	D3D11_INPUT_ELEMENT_DESC layout[ 3 ];
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
	layout[ 1 ].SemanticName = "NORMAL";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layout[ 2 ].InputSlot = 0;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 2 ].InstanceDataStepRate = 0;
	layout[ 2 ].SemanticIndex = 0;
	layout[ 2 ].SemanticName = "TEXCOORD";
	UINT layoutCount = ARRAYSIZE( layout );

	ThrowIfFailed( mDevice->CreateInputLayout( layout, layoutCount,
		mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ), &mLayout ) );
}

void ModelShader::CreateBuffers( )
{
	ShaderHelper::CreateBuffer( mDevice, &mObjectBufferVS,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SPerObjectVS ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
	ShaderHelper::CreateBuffer( mDevice, &mObjectBufferPS,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SPerObjectPS ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
	ShaderHelper::CreateBuffer( mDevice, &mLightBufferPS,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SLightPS ), 0 );
	ShaderHelper::CreateBuffer( mDevice, &mLightBufferVS,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof( SLightVS ), 0 );
}

void ModelShader::Render( UINT indexCount, ModelShader::SPerObjectVS const& argsVS, ModelShader::SPerObjectPS const& argsPS)
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;
	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );
	mContext->IASetInputLayout( mLayout.Get( ) );

	mContext->Map( mObjectBufferVS.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	memcpy_s( MappedSubresource.pData, sizeof( ModelShader::SPerObjectVS ), &argsVS, sizeof( ModelShader::SPerObjectVS ) );
	mContext->Unmap( mObjectBufferVS.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mObjectBufferVS.GetAddressOf( ) );
	mContext->VSSetConstantBuffers( 1, 1, mLightBufferVS.GetAddressOf( ) );

	mContext->Map( mObjectBufferPS.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	memcpy_s( MappedSubresource.pData, sizeof( ModelShader::SPerObjectPS ), &argsPS, sizeof( ModelShader::SPerObjectPS ) );
	mContext->Unmap( mObjectBufferPS.Get( ), 0 );
	mContext->PSSetConstantBuffers( 0, 1, mObjectBufferPS.GetAddressOf( ) );
	mContext->PSSetConstantBuffers( 1, 1, mLightBufferPS.GetAddressOf( ) );

	mContext->PSSetSamplers( 1, 1, DX::ComparisonLinearClampSampler.GetAddressOf( ) );

	ID3D11ShaderResourceView * SRV = const_cast< ID3D11ShaderResourceView* >( mShadowMap );
	mContext->PSSetShaderResources( 10, 1, &SRV );

	mContext->DrawIndexed( indexCount, 0, 0 );

	ID3D11ShaderResourceView * nullSRV = nullptr;
	mContext->PSSetShaderResources( 10, 1, &nullSRV );
}

void ModelShader::SetLight( SLightVS const& lightVS, SLightPS const& lightPS )
{
	mContext->UpdateSubresource( mLightBufferVS.Get( ), 0, nullptr, &lightVS, 0, 0 );
	mContext->UpdateSubresource( mLightBufferPS.Get( ), 0, nullptr, &lightPS, 0, 0 );
}

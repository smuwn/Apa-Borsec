#include "ProjectiveTexturingShader.h"



ProjectiveTexturingShader::ProjectiveTexturingShader( ID3D11Device * device, ID3D11DeviceContext * context ) :
	mDevice( device ),
	mContext( context )
{
	try
	{
		ID3D11VertexShader ** VS = &mVertexShader;
		ID3D11PixelShader ** PS = &mPixelShader;

		ShaderHelper::CreateShaderFromFile( L"Shaders/ProjectiveTexturingVertexShader.cso",
			"vs_4_0", mDevice, &mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
		ShaderHelper::CreateShaderFromFile( L"Shaders/ProjectiveTexturingPixelShader.cso",
			"ps_4_0", mDevice, &mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );

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
		int numLayout = ARRAYSIZE( layout );
		ThrowIfFailed(
			mDevice->CreateInputLayout( layout, numLayout,
				mBlobs[ 0 ]->GetBufferPointer( ), mBlobs[ 0 ]->GetBufferSize( ),
				&mLayout )
		);

		ShaderHelper::CreateBuffer( mDevice, &mConstantBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SConstantBuffer ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );
	}
	CATCH;
}


ProjectiveTexturingShader::~ProjectiveTexturingShader( )
{
}


void ProjectiveTexturingShader::Render( UINT indexCount, SConstantBuffer const& buffer,
	ID3D11ShaderResourceView * textureSRV )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	mContext->IASetInputLayout( mLayout.Get( ) );

	mContext->VSSetShader( mVertexShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mPixelShader.Get( ), nullptr, 0 );

	mContext->Map( mConstantBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	memcpy_s( MappedSubresource.pData, sizeof( SConstantBuffer ), ( void* ) &buffer, sizeof( SConstantBuffer ) );
	mContext->Unmap( mConstantBuffer.Get( ), 0 );
	mContext->VSSetConstantBuffers( 0, 1, mConstantBuffer.GetAddressOf( ) );

	mContext->PSSetShaderResources( 0, 1, &textureSRV );

	mContext->RSSetState( DX::NoCulling.Get( ) );
	mContext->DrawIndexed( indexCount, 0, 0 );
	mContext->RSSetState( DX::DefaultRS.Get( ) );

}
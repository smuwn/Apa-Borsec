#include "ParticleShader.h"




CParticleShader::CParticleShader( ID3D11Device * device, ID3D11DeviceContext * context ) :
	mDevice( device ),
	mContext( context )
{
	try
	{
		ShaderHelper::CreateBuffer( device, &mEmitterInfoBuffer,
			D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
			sizeof( SPerFrame ), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE );


	}
	CATCH;
}


CParticleShader::~CParticleShader( )
{
}


void CParticleShader::CreateStreamOutShaders( LPWSTR lpVertexShader, LPWSTR lpGeometryShader )
{
	ID3D11VertexShader ** VS = &mStreamOut.mVertexShader;
	ShaderHelper::CreateShaderFromFile( lpVertexShader, "vs_4_0", mDevice,
		&mStreamOut.mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );
	D3D11_INPUT_ELEMENT_DESC layout[ 5 ];

#pragma region UINT layoutSize
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
	layout[ 1 ].SemanticName = "DIRECTION";
	layout[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 2 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
	layout[ 2 ].InputSlot = 0;
	layout[ 2 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 2 ].InstanceDataStepRate = 0;
	layout[ 2 ].SemanticIndex = 0;
	layout[ 2 ].SemanticName = "SIZE";
	layout[ 3 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 3 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	layout[ 3 ].InputSlot = 0;
	layout[ 3 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 3 ].InstanceDataStepRate = 0;
	layout[ 3 ].SemanticIndex = 0;
	layout[ 3 ].SemanticName = "AGE";
	layout[ 4 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[ 4 ].Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	layout[ 4 ].InputSlot = 0;
	layout[ 4 ].InputSlotClass = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
	layout[ 4 ].InstanceDataStepRate = 0;
	layout[ 4 ].SemanticIndex = 0;
	layout[ 4 ].SemanticName = "TYPE";
	UINT layoutSize = ARRAYSIZE( layout );
	ThrowIfFailed(
		mDevice->CreateInputLayout( layout, layoutSize, mStreamOut.mBlobs[ 0 ]->GetBufferPointer( ),
			mStreamOut.mBlobs[ 0 ]->GetBufferSize( ), &mLayout )
		);
#pragma endregion Create layout

	D3D11_SO_DECLARATION_ENTRY *soe;

	ID3D11GeometryShader ** GS = &mStreamOut.mGeometryShader;
	ShaderHelper::CreateGeometryShaderWithStreamOutput( lpGeometryShader, "gs_4_0",
		mDevice, &mStreamOut.mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( GS ),
		layout, layoutSize, soe );

	delete[ ] soe;
}

void CParticleShader::RenderStreamOut( SPerFrame & info, UINT vertexCount )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	mContext->IASetInputLayout( mLayout.Get( ) );

	mContext->VSSetShader( mStreamOut.mVertexShader.Get( ), nullptr, 0 );
	mContext->GSSetShader( mStreamOut.mGeometryShader.Get( ), nullptr, 0 );

	// Disable rasterization
	mContext->OMSetDepthStencilState( DX::DSDisabled.Get( ), 0 );
	mContext->PSSetShader( nullptr, nullptr, 0 );

	mContext->Map( mEmitterInfoBuffer.Get( ), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &MappedSubresource );
	memcpy_s( MappedSubresource.pData, sizeof( SPerFrame ), ( void* ) &info, sizeof( SPerFrame ) );
	mContext->Unmap( mEmitterInfoBuffer.Get( ), 0 );
	mContext->GSSetConstantBuffers( 0, 1, mEmitterInfoBuffer.GetAddressOf( ) );
	mContext->GSSetSamplers( 0, 1, DX::PointWrapSampler.GetAddressOf( ) );

	ID3D11ShaderResourceView * SRVs[ ] = { Utilities::GetRandomTextureSRV( ) };
	mContext->GSSetShaderResources( 0, 1, SRVs );

	if ( vertexCount )
		mContext->Draw( vertexCount, 0 );
	else
		mContext->DrawAuto( );

	mContext->OMSetDepthStencilState( nullptr, 0 );

	mContext->VSSetShader( nullptr, nullptr, 0 );
	mContext->GSSetShader( nullptr, nullptr, 0 );

}

void CParticleShader::CreateRenderShaders( LPWSTR lpVertexShader, LPWSTR lpGeometryShader, LPWSTR lpPixelShader )
{
	ID3D11VertexShader ** VS = &mRender.mVertexShader;
	ShaderHelper::CreateShaderFromFile( lpVertexShader, "vs_4_0", mDevice,
		&mRender.mBlobs[ 0 ], reinterpret_cast< ID3D11DeviceChild** >( VS ) );

	ID3D11GeometryShader ** GS = &mRender.mGeometryShader;
	ShaderHelper::CreateShaderFromFile( lpGeometryShader, "gs_4_0", mDevice,
		&mRender.mBlobs[ 1 ], reinterpret_cast< ID3D11DeviceChild** >( GS ) );

	ID3D11PixelShader ** PS = &mRender.mPixelShader;
	ShaderHelper::CreateShaderFromFile( lpPixelShader, "ps_4_0", mDevice,
		&mRender.mBlobs[ 2 ], reinterpret_cast< ID3D11DeviceChild** >( PS ) );
}

void CParticleShader::Render( ID3D11ShaderResourceView * texture )
{
	static D3D11_MAPPED_SUBRESOURCE MappedSubresource;

	mContext->IASetInputLayout( mLayout.Get( ) );

	mContext->VSSetShader( mRender.mVertexShader.Get( ), nullptr, 0 );
	mContext->GSSetShader( mRender.mGeometryShader.Get( ), nullptr, 0 );
	mContext->PSSetShader( mRender.mPixelShader.Get( ), nullptr, 0 );

	mContext->GSSetConstantBuffers( 0, 1, mEmitterInfoBuffer.GetAddressOf( ) );
	
	mContext->PSSetShaderResources( 0, 1, &texture );
	mContext->PSSetSamplers( 0, 1, DX::PointWrapSampler.GetAddressOf( ) );

	mContext->RSSetState( DX::NoCulling.Get( ) );

	mContext->DrawAuto( );

	mContext->RSSetState( DX::DefaultRS.Get( ) );

	mContext->GSSetShader( nullptr, nullptr, 0 );
}

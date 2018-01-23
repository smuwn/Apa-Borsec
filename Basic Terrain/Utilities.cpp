#include "Utilities.h"


Microsoft::WRL::ComPtr<ID3D11Texture1D> Utilities::mRandomTexture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Utilities::mRandomTextureSRV = nullptr;



void Utilities::Create( ID3D11Device * device )
{
	ZeroMemoryAndDeclare( D3D11_TEXTURE1D_DESC, texDesc );
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.MipLevels = 1;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	texDesc.Width = RandomTextureSize;

	DirectX::XMFLOAT4 values[ RandomTextureSize ];
	for ( unsigned int i = 0; i < RandomTextureSize; ++i )
		values[ i ] = GetRandomDirection( );

	ZeroMemoryAndDeclare( D3D11_SUBRESOURCE_DATA, texData );
	texData.pSysMem = values;

	ThrowIfFailed(
		device->CreateTexture1D( &texDesc, &texData, &mRandomTexture )
		);

	ZeroMemoryAndDeclare( D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc );
	srvDesc.Format = texDesc.Format;
	srvDesc.Texture1D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture1D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE1D;
	ThrowIfFailed(
		device->CreateShaderResourceView( mRandomTexture.Get( ), &srvDesc, &mRandomTextureSRV )
		);

}

DirectX::XMFLOAT4 Utilities::GetRandomDirection( )
{
	DirectX::XMFLOAT4 returnValue;

	static DirectX::XMVECTOR One = DirectX::XMVectorSet( 1.0f, 1.0f, 1.0f, 1.0f );


	while ( true )
	{
		
		DirectX::XMVECTOR current = DirectX::XMVectorSet(
			DX::randomNumber( -1.0f, 1.0f ),
			DX::randomNumber( -1.0f, 1.0f ),
			DX::randomNumber( -1.0f, 1.0f ),
			0.0f
			);

		if ( DirectX::XMVector3Greater( current, One ) )
			continue;

		DirectX::XMStoreFloat4( &returnValue, DirectX::XMVector3Normalize( current ) );

		return returnValue;
	}

}

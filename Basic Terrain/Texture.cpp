#include "Texture.h"



CTexture::CTexture( LPWSTR lpPath, ID3D11Device* device )
{
	ThrowIfFailed(
		D3DX11CreateShaderResourceViewFromFile( device, lpPath, nullptr, nullptr, &mTextureSRV, nullptr )
		);
}


CTexture::~CTexture( )
{
	mTextureSRV.Reset( );
}

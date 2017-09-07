#include "Texture.h"



CTexture::CTexture( LPWSTR lpPath, ID3D11Device* device )
{
	DX::ThrowIfFailed(
		D3DX11CreateShaderResourceViewFromFile( device, lpPath, nullptr, nullptr, &mFPSTextureSRV, nullptr )
		);
}


CTexture::~CTexture( )
{
	mFPSTextureSRV.Reset( );
}

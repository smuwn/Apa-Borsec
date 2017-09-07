#pragma once

#include "commonincludes.h"

class CTexture
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mFPSTextureSRV;
public:
	CTexture( LPWSTR lpPath, ID3D11Device * );
	~CTexture( );
public:
	ID3D11ShaderResourceView* GetTexture()
	{
		return mFPSTextureSRV.Get( );
	}
};


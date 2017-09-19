#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

class CDefaultShader sealed
{
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;
	
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

public:
	CDefaultShader( ID3D11Device *, ID3D11DeviceContext * );
	~CDefaultShader( );
	
public:
	void Render( UINT indexCount );

};


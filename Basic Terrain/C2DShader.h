#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

class C2DShader
{
public:

	struct SPerObject
	{
		DirectX::XMMATRIX WP;
	};

private:

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	std::array < Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	C2DShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~C2DShader( );

public:
	void Render( UINT indexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& Projection );
};


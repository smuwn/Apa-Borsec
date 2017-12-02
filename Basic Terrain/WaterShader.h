#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Texture.h"

class CWaterShader
{
public:
	struct SVSPerObject
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
		DirectX::XMMATRIX ReflectView;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSPerObjectBuffer;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> mWrapSampler;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CWaterShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~CWaterShader( );
public:
	void Render( UINT indexCount,
		DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View,
		DirectX::FXMMATRIX& Projection, DirectX::FXMMATRIX& ReflectView,
		CTexture * Reflection, CTexture * Refraction );
};


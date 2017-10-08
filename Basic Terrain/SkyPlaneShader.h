#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Texture.h"


class SkyPlaneShader
{
public:
	struct SVSPerObject
	{
		DirectX::XMMATRIX WVP;
	};
	struct SPSPerObject
	{
		DirectX::XMFLOAT2 FirstTextureOffset;
		DirectX::XMFLOAT2 SecondTextureOffset;
		float brightness;
		DirectX::XMFLOAT3 pad;
	};
	struct STextureInfo
	{
		CTexture * Texture;
		DirectX::XMFLOAT2 Offset;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPSPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> mWrapSampler;


	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
public:
	SkyPlaneShader( ID3D11Device*, ID3D11DeviceContext* );
	~SkyPlaneShader( );
public:
	void Render( UINT IndexCount, float brightness,
		DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		STextureInfo const& FirstTexture, STextureInfo const& SecondTexture );
};


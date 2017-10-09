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
	struct STextureInfo
	{
		DirectX::XMFLOAT2 translation;
		float scale;
		float brightness;
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
	void Render( UINT IndexCount, STextureInfo const& TextureInfo,
		DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		CTexture const* CloudTexture, CTexture const* PerturbTexture );
};


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
	struct SVSCamera
	{
		DirectX::XMFLOAT3 CamPos;
		float pad;
		DirectX::XMFLOAT2 NormalMapTiling;
		DirectX::XMFLOAT2 pad1;
	};
	struct SPSWater
	{
		DirectX::XMFLOAT4 WaterColor;
		float WaterShininess;
		float reflectRefractScale;
		DirectX::XMFLOAT2 pad;
	};
	struct SPSPerObject
	{
		float waterTranslation;
		DirectX::XMFLOAT3 lightDirection;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCameraBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPSWaterBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPSPerObjectBuffer;

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
		CTexture * Reflection, CTexture * Refraction, CTexture * Normals,
		DirectX::XMFLOAT3 const& CamPos, DirectX::XMFLOAT2 const& TextureRepeat,
		DirectX::XMFLOAT3 const& LightDir, float waterTranslation );
	void SetWaterInfo( SPSWater const& Water );
};


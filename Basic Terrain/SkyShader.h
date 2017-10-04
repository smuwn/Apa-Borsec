#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class SkyShader
{
public:
	struct SVSPerObject
	{
		DirectX::XMMATRIX WVP;
	};
	struct SPSColor
	{
		DirectX::XMFLOAT4 CenterColor;
		DirectX::XMFLOAT4 ApexColor;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mPSColorBuffer;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	SkyShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~SkyShader( );
public:
	void Render( UINT IndexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
	void RenderVertices( UINT VertexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
	void SetColors( SPSColor const& Color );
};


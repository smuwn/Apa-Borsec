#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Texture.h"

class C3DShader sealed
{
public:
	struct SPerObject
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX World;
	};
	struct SLight
	{
		DirectX::XMFLOAT3 Dir;
		float pad;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT4 Ambient;
	};
	struct STexture
	{
		BOOL HasAlpha;
		DirectX::XMFLOAT3 Pad;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	std::array < Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mPerObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTextureBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> mWrapSampler;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	C3DShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~C3DShader( );
public:
	void Render( UINT IndexCount, DirectX::FXMMATRIX& World,
		DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		CTexture * texture );
	void RenderVertices( UINT vertexCount, DirectX::FXMMATRIX& World,
		DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		CTexture * texture, CTexture * = nullptr, CTexture * = nullptr );
	void SetLight( SLight const& Light );
};


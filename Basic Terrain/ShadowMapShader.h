#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class ShadowMapShader
{
public:
	struct RenderVertices { };
	struct RenderIndices { };
public:
	struct SObjectBuffer
	{
		DirectX::XMMATRIX WVP;
	};
	struct SMaterialBuffer
	{
		BOOL HasTexture;
		DirectX::XMFLOAT3 Pad;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mObjectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mMaterialBuffer;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	ShadowMapShader( ID3D11Device * device, ID3D11DeviceContext * context );
	~ShadowMapShader( );
private:
	void InitializeShaders( );
	void InitializeBuffers( );
public:
	void __vectorcall RenderIndices( UINT indexCount, DirectX::FXMMATRIX& WVP,
		BOOL bHasTexture, ID3D11ShaderResourceView * srv = nullptr );
	void __vectorcall RenderVertices( UINT vertexCount, DirectX::FXMMATRIX& WVP,
		BOOL bHasTexture, ID3D11ShaderResourceView * srv = nullptr );
private:
	void __vectorcall PrepareForRendering( DirectX::FXMMATRIX& WVP,
		BOOL bHasTexture, ID3D11ShaderResourceView * srv );
};

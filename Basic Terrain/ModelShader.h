#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class ModelShader
{
public:
	struct SPerObjectVS
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
	struct SPerObjectPS
	{
		DirectX::XMFLOAT4 Color;
	};
	struct SLightPS
	{
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT3 Position;
		float pad;
	};
	struct SLightVS
	{
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mObjectBufferVS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mObjectBufferPS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mLightBufferPS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mLightBufferVS;

	ID3D11ShaderResourceView const * const mShadowMap = nullptr;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	ModelShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~ModelShader( );
public:
	void SetShadowMap( ID3D11ShaderResourceView * shadowMap )
	{
		ID3D11ShaderResourceView ** srv = const_cast< ID3D11ShaderResourceView** >( &mShadowMap );
		*srv = shadowMap;
	}
public:
	void Render( UINT indexCount, SPerObjectVS const&, SPerObjectPS const&);
	void SetLight( SLightVS const&, SLightPS const& );
private:
	void CreateShaders( );
	void CreateBuffers( );
};


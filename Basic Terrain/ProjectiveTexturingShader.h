#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class ProjectiveTexturingShader
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 Texture;
	};
	struct SConstantBuffer
	{
		DirectX::XMMATRIX World;

		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;

		DirectX::XMMATRIX ProjectorView;
		DirectX::XMMATRIX ProjectorProjection;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	std::array<ID3DBlob*, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;;
	
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	ProjectiveTexturingShader( ID3D11Device * device, ID3D11DeviceContext * context );
	~ProjectiveTexturingShader( );
public:
	void Render( UINT indexCount, SConstantBuffer const& buffer,
		ID3D11ShaderResourceView * textureSRV );
};


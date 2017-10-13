#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"


class DepthShader
{
public:
	struct SVSPerObject
	{
		DirectX::XMMATRIX WVP;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
	std::array< Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSPerObjectBuffer;


	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	DepthShader( ) = delete;
	DepthShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~DepthShader( );
public:
	void Render( UINT indexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
	void RenderVertices( UINT vertexCount, DirectX::FXMMATRIX& World, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
};


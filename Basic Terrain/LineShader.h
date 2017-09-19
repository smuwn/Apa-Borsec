#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

class LineShader sealed
{
public:
	struct SPerObject
	{
		DirectX::XMMATRIX WVP;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
	std::array < Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mPerObjectBuffer;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	LineShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~LineShader( );
public:
	void Render( int VertexCount, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
};


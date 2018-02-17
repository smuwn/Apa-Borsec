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
	struct SClippingPlane
	{
		DirectX::XMFLOAT4 Plane;
	};
	struct SLightVS
	{
		DirectX::XMMATRIX ViewProjection;
	};
	struct SLightPS
	{
		DirectX::XMFLOAT3 Pos;
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
	Microsoft::WRL::ComPtr<ID3D11Buffer> mClippingPlaneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mLightVSBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mLightPSBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mTextureBuffer;
	
	ID3D11ShaderResourceView const * const mShadowMap = nullptr;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	C3DShader( ID3D11Device * Device, ID3D11DeviceContext * Context );
	~C3DShader( );
public:
	void SetShadowMap( ID3D11ShaderResourceView * shadowMap )
	{
		ID3D11ShaderResourceView ** srv = const_cast< ID3D11ShaderResourceView** >( &mShadowMap );
		*srv = shadowMap;
	}
public:
	void Render( UINT IndexCount, DirectX::FXMMATRIX& World,
		DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		CTexture * texture );
	void RenderVertices( UINT vertexCount, DirectX::FXMMATRIX& World,
		DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		CTexture * texture, CTexture * = nullptr, CTexture * = nullptr,
		int = 1 );
	void SetLight( SLightVS const&, SLightPS const& );
	void SetClippingPlane( SClippingPlane const& ClippingPlane );
};


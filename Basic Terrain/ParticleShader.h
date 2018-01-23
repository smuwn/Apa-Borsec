#pragma once


#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Utilities.h"



class CParticleShader
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 ParticlePos;
		DirectX::XMFLOAT3 ParticleDir;

		DirectX::XMFLOAT2 Size;
		
		float Age;
		unsigned int Type;
	};
	struct SPerFrame
	{
		DirectX::XMFLOAT3 gEyePosW;
		float pad;

		DirectX::XMFLOAT3 gEmitPosW;
		float gSystemLifeTime;
		DirectX::XMFLOAT3 gEmitDirW;
		float gDeltaTime;

		DirectX::XMMATRIX gViewProj;
	};
private:

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mEmitterInfoBuffer;

	struct
	{
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGeometryShader;

		std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 2> mBlobs;
	} mStreamOut;
	
	struct
	{
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;

		std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 3> mBlobs;
	} mRender;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

public:
	CParticleShader( ID3D11Device * device, ID3D11DeviceContext * context );
	~CParticleShader( );
public:
	void CreateStreamOutShaders( LPWSTR lpVertexShader, LPWSTR lpGeometryShader );
	void RenderStreamOut( SPerFrame& info, UINT vertexCount = 0 );
	void CreateRenderShaders( LPWSTR lpVertexShader, LPWSTR lpGeometryShader, LPWSTR lpPixelShader );
	void Render( ID3D11ShaderResourceView * ); /// To be used afer RenderStreamOut
};


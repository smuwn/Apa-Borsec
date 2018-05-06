#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "TextureUtilities.h"



class ButterflySolver
{
	static constexpr const unsigned int TextureSize = 256;
	struct SSolverInfo
	{
		int stage;
		int direction;
		DirectX::XMFLOAT2 pad;
	};

public:
	ButterflySolver(ID3D11Device * device, ID3D11DeviceContext * context);
	~ButterflySolver();

public:
	void Compute(ID3D11ShaderResourceView * twiddleIndices,
		ID3D11ShaderResourceView * dySRV, ID3D11UnorderedAccessView * dyUAV);
	inline ID3D11ShaderResourceView * GetHeightmap() const
	{
		return mFinalResultTextureSRV.Get();
	}
private:
	void CreateShader();
	void CreateTexture();
	void CreateAdditional();

	void UpdateBuffer(int direction, int stage);
	void SetResources(int pingpong, ID3D11ShaderResourceView * srv, ID3D11UnorderedAccessView * uav);
private:
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mComputeShader;
	Microsoft::WRL::ComPtr<ID3DBlob> mComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mInverseComputeShader;
	Microsoft::WRL::ComPtr<ID3DBlob> mInverseComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mFinalResultTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mFinalResultTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mFinalResultTextureUAV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mSolverInfoBuffer;

	int mLog2N = (int)(log(TextureSize) / log(2));
};


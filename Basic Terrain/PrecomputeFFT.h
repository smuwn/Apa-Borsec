#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Texture.h"

class PrecomputeFFT
{
public:
	struct SInfo
	{
		int N;
		int L;
		float A;
		DirectX::XMFLOAT2 windDir;
		float windSpeed;
		DirectX::XMFLOAT2 pad;
	};
	static constexpr const unsigned int TextureSize = 256;
public:
	PrecomputeFFT(ID3D11Device *, ID3D11DeviceContext *);
	~PrecomputeFFT();

public:
	void Compute();

	inline ID3D11ShaderResourceView * GetH0() const
	{
		return mTextureHSRV.Get();
	}
	inline ID3D11ShaderResourceView * GetH0Minus() const
	{
		return mTextureMinusHSRV.Get();
	}
	inline SInfo  const & GetInfo() const
	{
		return mInfo;
	}

private:
	void CreateShaders();
	void CreateNoiseTextures();
	void CreateAdditional();

private:
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

	SInfo mInfo;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mComputeShader;
	Microsoft::WRL::ComPtr<ID3DBlob> mComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mInfoBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureH;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureHSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureHUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureMinusH;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureMinusHSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureMinusHUAV;

	std::unique_ptr<CTexture> mNoise0;
	std::unique_ptr<CTexture> mNoise1;
	std::unique_ptr<CTexture> mNoise2;
	std::unique_ptr<CTexture> mNoise3;
};


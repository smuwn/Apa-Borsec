#pragma once


#include "commonincludes.h"
#include "ShaderHelper.h"
#include "Texture.h"
#include "PrecomputeFFT.h"
#include "TextureUtilities.h"


class TimeDependentFFT
{
	static constexpr const unsigned int TextureSize = 256;
	struct SInfo
	{
		int N;
		int L;
		float Time;
		float pad;
	};
public:
	TimeDependentFFT(ID3D11Device *, ID3D11DeviceContext *);
	~TimeDependentFFT();

public:
	void Compute(float Time, PrecomputeFFT::SInfo const& info);

	inline ID3D11ShaderResourceView * GetTextureSRVDX() const
	{
		return mTextureDXSRV.Get();
	}

	inline ID3D11ShaderResourceView * GetTextureSRVDY() const
	{
		return mTextureDYSRV.Get();
	}

	inline ID3D11ShaderResourceView * GetTextureSRVDZ() const
	{
		return mTextureDZSRV.Get();
	}

	inline void SetComponents(ID3D11ShaderResourceView * h0k, ID3D11ShaderResourceView * h0minusk)
	{
		mH0SRV = h0k;
		mH0MinusSRV = h0minusk;
	}
private:
	void CreateShader();
	void CreateTextures();
	void CreateAdditional();

private:
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

	ID3D11ShaderResourceView * mH0SRV;
	ID3D11ShaderResourceView * mH0MinusSRV;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mComputeShader;
	Microsoft::WRL::ComPtr<ID3DBlob> mComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mInfoBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureDX;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureDXSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureDXUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureDY;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureDYSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureDYUAV;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTextureDZ;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureDZSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTextureDZUAV;
};


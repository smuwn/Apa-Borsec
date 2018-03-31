#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"
#include "TextureUtilities.h"


class ComputeTwiddleIndices
{
	static constexpr const unsigned int TextureSize = 256;
	struct SIndex
	{
		int index;
		DirectX::XMFLOAT3 pad;
	};
public:
	ComputeTwiddleIndices(ID3D11Device * device, ID3D11DeviceContext * context);
	~ComputeTwiddleIndices();

public:
	void Compute();
	
	inline ID3D11ShaderResourceView * GetTwiddleTexture() const
	{
		return mTwiddleIndicesTextureSRV.Get();
	}

private:
	void CreateShader();
	void CreateTexture();
	void CreateAdditional();
	void InitReversedIndices();

private:
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

	int mLog2N;
	std::vector<SIndex> mReversedIndices;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mComputeShader;
	Microsoft::WRL::ComPtr<ID3DBlob> mComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndicesBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTwiddleIndicesTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTwiddleIndicesTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mTwiddleIndicesTextureUAV;
};


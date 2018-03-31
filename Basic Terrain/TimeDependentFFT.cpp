#include "TimeDependentFFT.h"



TimeDependentFFT::TimeDependentFFT(ID3D11Device * device, ID3D11DeviceContext * context) :
	mDevice(device),
	mContext(context)
{
	try
	{
		CreateShader();
		CreateTextures();
		CreateAdditional();
	}
	CATCH;
}


TimeDependentFFT::~TimeDependentFFT()
{
}

void TimeDependentFFT::Compute(float Time, PrecomputeFFT::SInfo const & info)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	mContext->Map(mInfoBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	((SInfo*)mappedSubresource.pData)->L = info.L;
	((SInfo*)mappedSubresource.pData)->N = info.N;
	((SInfo*)mappedSubresource.pData)->Time = Time;
	mContext->Unmap(mInfoBuffer.Get(), 0);
	mContext->CSSetConstantBuffers(0, 1, mInfoBuffer.GetAddressOf());
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);

	ID3D11ShaderResourceView * SRVs[] =
	{
		mH0SRV,
		mH0MinusSRV
	};
	mContext->CSSetShaderResources(0, 2, SRVs);

	ID3D11UnorderedAccessView * UAVs[] =
	{
		mTextureDXUAV.Get(),
		mTextureDYUAV.Get(),
		mTextureDZUAV.Get()
	};
	mContext->CSSetUnorderedAccessViews(0, 3, UAVs, nullptr);

	UINT threadGroupX = (UINT)ceil(TextureSize / 32.0f);
	UINT threadGroupY = (UINT)ceil(TextureSize / 32.0f);
	UINT threadGroupZ = 1;
	mContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	ID3D11UnorderedAccessView * nullUAV[] =
	{
		nullptr, nullptr, nullptr
	};
	mContext->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);
}

void TimeDependentFFT::CreateShader()
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(L"Shaders/FFTComponentsComputeShader.cso", "cs_5_0",
		mDevice, &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
}

void TimeDependentFFT::CreateTextures()
{
	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS |
		D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = TextureSize;
	texDesc.Height = TextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mTextureDX)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTextureDX.Get(), &mTextureDXSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTextureDX.Get(), &mTextureDXUAV);

	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mTextureDY)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTextureDY.Get(), &mTextureDYSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTextureDY.Get(), &mTextureDYUAV);

	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mTextureDZ)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTextureDZ.Get(), &mTextureDZSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTextureDZ.Get(), &mTextureDZUAV);
}

void TimeDependentFFT::CreateAdditional()
{
	ShaderHelper::CreateBuffer(mDevice, &mInfoBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SInfo), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE);
}
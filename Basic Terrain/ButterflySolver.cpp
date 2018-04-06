#include "ButterflySolver.h"



ButterflySolver::ButterflySolver(ID3D11Device * device, ID3D11DeviceContext * context) :
	mDevice(device),
	mContext(context)
{
	try
	{
		CreateShader();
		CreateTexture();
		CreateAdditional();
	}
	CATCH;
}


ButterflySolver::~ButterflySolver()
{
}

void ButterflySolver::CreateShader()
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(L"Shaders/ButterflySolver.cso", "cs_5_0",
		mDevice, &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));

	CS = &mInverseComputeShader;
	ShaderHelper::CreateShaderFromFile(L"Shaders/InversionComputeShader.cso", "cs_5_0",
		mDevice, &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
}

void ButterflySolver::CreateTexture()
{
	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Height = TextureSize;
	texDesc.Width = TextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mTexture)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTexture.Get(), &mTextureSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTexture.Get(), &mTextureUAV);

	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mFinalResultTexture)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTexture.Get(), &mFinalResultTextureSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTexture.Get(), &mFinalResultTextureUAV);
}

void ButterflySolver::CreateAdditional()
{
	ShaderHelper::CreateBuffer(
		mDevice, &mSolverInfoBuffer,
		D3D11_USAGE::D3D11_USAGE_DYNAMIC,
		D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SSolverInfo), D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE
	);
}

void ButterflySolver::UpdateBuffer(int direction, int stage)
{
	static D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	mContext->Map(mSolverInfoBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	((SSolverInfo*)mappedSubresource.pData)->direction = direction;
	((SSolverInfo*)mappedSubresource.pData)->stage = stage;
	mContext->Unmap(mSolverInfoBuffer.Get(), 0);
	mContext->CSSetConstantBuffers(0, 1, mSolverInfoBuffer.GetAddressOf());
}

void ButterflySolver::SetResources(int pingpong, ID3D11ShaderResourceView * srv, ID3D11UnorderedAccessView * uav)
{
	if (pingpong % 2)
	{
		mContext->CSSetShaderResources(1, 1, mTextureSRV.GetAddressOf());
		mContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	}
	else
	{
		mContext->CSSetShaderResources(1, 1, &srv);
		mContext->CSSetUnorderedAccessViews(0, 1, mTextureUAV.GetAddressOf(), nullptr);
	}
}

void ButterflySolver::Compute(ID3D11ShaderResourceView * twiddleindices,
	ID3D11ShaderResourceView * dySRV,ID3D11UnorderedAccessView * dyUAV)
{
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);

	mContext->CSSetShaderResources(0, 1, &twiddleindices);

	ID3D11UnorderedAccessView * nullUAVs[] =
	{
		nullptr, nullptr, nullptr
	};

	ID3D11ShaderResourceView * nullSRVs[] =
	{
		nullptr, nullptr, nullptr
	};

	int pingpong = 0;
	for (int i = 0; i < mLog2N; ++i)
	{
		SetResources(pingpong, dySRV, dyUAV);
		UpdateBuffer(0, i);
		int threadsX = (int)ceil((float)TextureSize / 16.0f);
		int threadsY = (int)ceil((float)TextureSize / 16.0f);
		int threadsZ = 1;
		mContext->Dispatch(threadsX, threadsY, threadsZ);

		mContext->CSSetUnorderedAccessViews(0, 3, nullUAVs, nullptr);

		pingpong++;
		pingpong = pingpong % 2;
	}

	for (int i = 0; i < mLog2N; ++i)
	{
		SetResources(pingpong, dySRV, dyUAV);
		UpdateBuffer(1, i);
		int threadsX = (int)ceil((float)TextureSize / 16.0f);
		int threadsY = (int)ceil((float)TextureSize / 16.0f);
		int threadsZ = 1;
		mContext->Dispatch(threadsX, threadsY, threadsZ);

		mContext->CSSetUnorderedAccessViews(0, 3, nullUAVs, nullptr);

		pingpong++;
		pingpong = pingpong % 2;
	}

	mContext->CSSetShader(mInverseComputeShader.Get(), nullptr, 0);
	mContext->CSSetShaderResources(0, 3, nullSRVs);
	mContext->CSSetUnorderedAccessViews(0, 3, nullUAVs, nullptr);

	if (pingpong == 0)
	{
		mContext->CSSetShaderResources(0, 1, &dySRV);
	}
	else
	{
		mContext->CSSetShaderResources(0, 1, mTextureSRV.GetAddressOf());
	}

	mContext->CSSetUnorderedAccessViews(0, 1, mFinalResultTextureUAV.GetAddressOf(), nullptr);

	int threadsX = (int)ceil((float)TextureSize / 32.0f);
	int threadsY = (int)ceil((float)TextureSize / 32.0f);
	int threadsZ = 1;
	mContext->Dispatch(threadsX, threadsY, threadsZ);

	mContext->CSSetShader(nullptr, nullptr, 0);
	mContext->CSSetUnorderedAccessViews(0, 3, nullUAVs, nullptr);
}
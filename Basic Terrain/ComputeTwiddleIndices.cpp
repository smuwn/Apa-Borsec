#include "ComputeTwiddleIndices.h"



ComputeTwiddleIndices::ComputeTwiddleIndices(ID3D11Device * device, ID3D11DeviceContext * context) :
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


ComputeTwiddleIndices::~ComputeTwiddleIndices()
{
}

void ComputeTwiddleIndices::Compute()
{
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);

	mContext->CSSetConstantBuffers(0, 1, mIndicesBuffer.GetAddressOf());
	mContext->CSSetUnorderedAccessViews(0, 1, mTwiddleIndicesTextureUAV.GetAddressOf(), nullptr);

	UINT threadGroupX = (UINT)ceil(TextureSize / 2.0f);
	UINT threadGroupY = (UINT)ceil(TextureSize / 512.0f);
	UINT threadGroupZ = 1;
	mContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);

	D3DX11SaveTextureToFile(mContext, mTwiddleIndicesTexture.Get(), D3DX11_IMAGE_FILE_FORMAT::D3DX11_IFF_PNG, L"Twiddle.png");

	ID3D11UnorderedAccessView * nullUAV[] = 
	{
		nullptr
	};
	mContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

	mContext->CSSetShader(nullptr, nullptr, 0);
}


void ComputeTwiddleIndices::CreateShader()
{
	ID3D11ComputeShader ** CS = &mComputeShader;
	ShaderHelper::CreateShaderFromFile(L"Shaders/TwiddleIndicesComputeShader.cso", "cs_5_0",
		mDevice, &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
	InitReversedIndices();
}

void ComputeTwiddleIndices::CreateTexture()
{
	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = mLog2N;
	texDesc.Height = TextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	ThrowIfFailed(
		mDevice->CreateTexture2D(&texDesc, nullptr, &mTwiddleIndicesTexture)
	);
	TextureUtilities::CreateSRVFromTexture(mDevice, mTwiddleIndicesTexture.Get(), &mTwiddleIndicesTextureSRV);
	TextureUtilities::CreateUAVFromTexture(mDevice, mTwiddleIndicesTexture.Get(), &mTwiddleIndicesTextureUAV);
}

void ComputeTwiddleIndices::CreateAdditional()
{
	ShaderHelper::CreateBuffer(mDevice, &mIndicesBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SIndex) * TextureSize, 0, &mReversedIndices[0]);
}

void ComputeTwiddleIndices::InitReversedIndices()
{
	mReversedIndices.resize(TextureSize);
	int bits = (int)(log(TextureSize) / log(2));
	mLog2N = bits;

	for (int i = 0; i < TextureSize; ++i)
	{
		int x = Math::reverseInt(i);
		x = Math::rotateLeft(x, bits);
		mReversedIndices[i].index = x;
	}
}
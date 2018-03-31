#include "PrecomputeFFT.h"



PrecomputeFFT::PrecomputeFFT(ID3D11Device * device, ID3D11DeviceContext * context) :
	mDevice(device),
	mContext(context)
{
	try
	{
		CreateShaders();
		CreateNoiseTextures();
		CreateAdditional();
		mInfo.N = TextureSize;
		mInfo.L = 1000;
		mInfo.A = 26;
		mInfo.windDir = DirectX::XMFLOAT2(1.0f, 0.0f);
		mInfo.windSpeed = 26;
	}
	CATCH;
}


PrecomputeFFT::~PrecomputeFFT()
{
}

void PrecomputeFFT::Compute()
{
	SInfo info;
	info.N = TextureSize;
	info.L = 1000;
	info.A = 26;
	info.windDir = DirectX::XMFLOAT2(1.0f, 0.0f);
	info.windSpeed = 26;
	mContext->UpdateSubresource(mInfoBuffer.Get(), 0, nullptr, &info, 0, 0);
	mContext->CSSetConstantBuffers(0, 1, mInfoBuffer.GetAddressOf());
		
	mContext->CSSetShader(mComputeShader.Get(), nullptr, 0);

	ID3D11ShaderResourceView * noise[] =
	{
		mNoise0->GetTexture(),
		mNoise1->GetTexture(),
		mNoise2->GetTexture(),
		mNoise3->GetTexture(),
	};
	mContext->CSSetShaderResources(0, 4, noise);

	ID3D11UnorderedAccessView * results[] =
	{
		mTextureHUAV.Get(),
		mTextureMinusHUAV.Get()
	};
	mContext->CSSetUnorderedAccessViews(0, 2, results, nullptr);

	UINT threadGroupX = (UINT)ceil(TextureSize / 32.0f);
	UINT threadGroupY = (UINT)ceil(TextureSize / 32.0f);
	UINT threadGroupZ = 1;
	mContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);

	mContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView * nullUAV[] =
	{
		nullptr, nullptr
	};
	mContext->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
}

void PrecomputeFFT::CreateShaders()
{
	ID3D11ComputeShader ** CS = &mComputeShader;

	ShaderHelper::CreateShaderFromFile(L"Shaders/Compute~h0.cso", "cs_5_0",
		mDevice, &mComputeShaderBlob, reinterpret_cast<ID3D11DeviceChild**>(CS));
}

void PrecomputeFFT::CreateNoiseTextures()
{
	mNoise0 = std::make_unique<CTexture>((LPWSTR)L"Data/Noise/Noise256_0.jpg", mDevice);
	mNoise1 = std::make_unique<CTexture>((LPWSTR)L"Data/Noise/Noise256_1.jpg", mDevice);
	mNoise2 = std::make_unique<CTexture>((LPWSTR)L"Data/Noise/Noise256_2.jpg", mDevice);
	mNoise3 = std::make_unique<CTexture>((LPWSTR)L"Data/Noise/Noise256_3.jpg", mDevice);
}

void PrecomputeFFT::CreateAdditional()
{
	ShaderHelper::CreateBuffer(mDevice, &mInfoBuffer,
		D3D11_USAGE::D3D11_USAGE_DEFAULT, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER,
		sizeof(SInfo), 0);

	ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = TextureSize;
	texDesc.Height = TextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	mDevice->CreateTexture2D(&texDesc, nullptr, &mTextureH);

	ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	ThrowIfFailed(
		mDevice->CreateShaderResourceView(mTextureH.Get(), &srvDesc, &mTextureHSRV)
	);

	ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		mDevice->CreateUnorderedAccessView(mTextureH.Get(), &uavDesc, &mTextureHUAV)
	);

	ZeroVariable(texDesc);
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
		D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = TextureSize;
	texDesc.Height = TextureSize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	mDevice->CreateTexture2D(&texDesc, nullptr, &mTextureMinusH);

	ZeroVariable(srvDesc);
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	ThrowIfFailed(
		mDevice->CreateShaderResourceView(mTextureMinusH.Get(), &srvDesc, &mTextureMinusHSRV)
	);

	ZeroVariable(uavDesc);
	uavDesc.Format = texDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	ThrowIfFailed(
		mDevice->CreateUnorderedAccessView(mTextureMinusH.Get(), &uavDesc, &mTextureMinusHUAV)
	);
}
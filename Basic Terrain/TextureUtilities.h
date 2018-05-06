#pragma once

#include "commonincludes.h"
#include "ShaderHelper.h"

namespace TextureUtilities
{

	inline void CreateUAVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11UnorderedAccessView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_UNORDERED_ACCESS_VIEW_DESC, uavDesc);
		uavDesc.Format = texDesc.Format;
		uavDesc.Texture2D.MipSlice = 0;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateUnorderedAccessView(texture, &uavDesc, finalResult)
		);
	}

	inline void CreateSRVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11ShaderResourceView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
		srvDesc.Format = texDesc.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateShaderResourceView(texture, &srvDesc, finalResult)
		);
	}

	inline void CreateRTVFromTexture(ID3D11Device * device, ID3D11Texture2D * texture,
		ID3D11RenderTargetView ** finalResult)
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texture->GetDesc(&texDesc);

		ZeroMemoryAndDeclare(D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc);
		rtvDesc.Format = texDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(
			device->CreateRenderTargetView(texture, &rtvDesc, finalResult)
		);
	}

	inline void CreateTextureAndViews(ID3D11Device* device, UINT width, UINT height, DXGI_FORMAT format,
		ID3D11Texture2D ** ppTex, ID3D11ShaderResourceView ** ppSRV, ID3D11UnorderedAccessView ** ppUAV,
		ID3D11RenderTargetView ** ppRTV)
	{
		int bindFlag = 0;
		if (ppSRV)
			bindFlag |= D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
		if (ppUAV)
			bindFlag |= D3D11_BIND_FLAG::D3D11_BIND_UNORDERED_ACCESS;
		if (ppRTV)
			bindFlag |= D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		ZeroMemoryAndDeclare(D3D11_TEXTURE2D_DESC, texDesc);
		texDesc.ArraySize = 1;
		texDesc.BindFlags = bindFlag;
		texDesc.Format = format;
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		ThrowIfFailed(
			device->CreateTexture2D(&texDesc, nullptr, ppTex)
		);
		if (ppSRV)
			CreateSRVFromTexture(device, *ppTex, ppSRV);
		if (ppUAV)
			CreateUAVFromTexture(device, *ppTex, ppUAV);
		if (ppRTV)
			CreateRTVFromTexture(device, *ppTex, ppRTV);
	}

}
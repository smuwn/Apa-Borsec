#pragma once

#include "Projector.h"


template <class ProjectionType>
class BuildShadowMap : public Projector<ProjectionType>
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mTextureDepth;

#if (DEBUG || _DEBUG) && ENABLE_SHADOW_DEBUG
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mDebugTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mDebugTextureRTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDebugTextureSRV;
#endif

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizer;

	float mShadowmapSize;

	D3D11_VIEWPORT mViewport;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	BuildShadowMap( ID3D11Device * device, ID3D11DeviceContext * context,
		float ShadowmapSize = 2048.f ) :
		Projector( ),
		mDevice( device ),
		mContext( context ),
		mShadowmapSize( ShadowmapSize )
	{
		try
		{
			mViewport.TopLeftX = 0;
			mViewport.TopLeftY = 0;
			mViewport.Height = mShadowmapSize;
			mViewport.Width = mShadowmapSize;
			mViewport.MinDepth = 0.0f;
			mViewport.MaxDepth = 1.0f;
			InitializeViews( );
			ZeroMemoryAndDeclare( D3D11_RASTERIZER_DESC, rastDesc );
			rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
			rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
			rastDesc.DepthBias = 10;
			rastDesc.DepthBiasClamp = 0.0f;
			rastDesc.SlopeScaledDepthBias = 2.f;
			ThrowIfFailed( mDevice->CreateRasterizerState( &rastDesc, &mRasterizer ) );
		}
		CATCH;
	}
	~BuildShadowMap( ) = default;
public:
	void PrepareForRendering( )
	{
		mContext->RSSetViewports( 1, &mViewport );
#if (DEBUG || _DEBUG) && ENABLE_SHADOW_DEBUG
		mContext->OMSetRenderTargets( 1, mDebugTextureRTV.GetAddressOf( ), mTextureDepth.Get( ) );
#else
		ID3D11RenderTargetView * nullRTV[ ] = { nullptr };
		mContext->OMSetRenderTargets( 0, nullptr, mTextureDepth.Get( ) );
#endif
		mContext->RSSetState( mRasterizer.Get( ) );
	};
	void ClearBuffer( )
	{
#if (DEBUG || _DEBUG) && ENABLE_SHADOW_DEBUG
		FLOAT color[ 4 ] = { 0.0f,0.0f,0.0f,0.0f };
		mContext->ClearRenderTargetView( mDebugTextureRTV.Get( ), color );
#endif
		mContext->ClearDepthStencilView( mTextureDepth.Get( ), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0 );
	};
	ID3D11ShaderResourceView * GetShadowMapSRV( )
	{
		return mTextureSRV.Get( );
	};
#if (DEBUG || _DEBUG) && ENABLE_SHADOW_DEBUG
	ID3D11ShaderResourceView * GetDebugShadowMapSRV( )
	{
		return mDebugTextureSRV.Get( );
	};
#endif
private:
	void InitializeViews( )
	{
		ZeroMemoryAndDeclare( D3D11_TEXTURE2D_DESC, texDesc );
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.Width = ( UINT ) mShadowmapSize;
		texDesc.Height = ( UINT ) mShadowmapSize;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		ThrowIfFailed( mDevice->CreateTexture2D( &texDesc, nullptr, &mTexture ) );

		ZeroMemoryAndDeclare( D3D11_DEPTH_STENCIL_VIEW_DESC, dsDesc );
		dsDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.Texture2D.MipSlice = 0;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;
		ThrowIfFailed( mDevice->CreateDepthStencilView( mTexture.Get( ), &dsDesc, &mTextureDepth ) );

		ZeroMemoryAndDeclare( D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc );
		srvDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		ThrowIfFailed( mDevice->CreateShaderResourceView( mTexture.Get( ), &srvDesc, &mTextureSRV ) );

#if (DEBUG || _DEBUG) && ENABLE_SHADOW_DEBUG
		ZeroVariable( texDesc );
		texDesc.ArraySize = 1;
		texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE |
			D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
		texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Width = ( UINT ) mShadowmapSize;
		texDesc.Height = ( UINT ) mShadowmapSize;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		ThrowIfFailed( mDevice->CreateTexture2D( &texDesc, nullptr, &mDebugTexture ) );

		ZeroVariable( srvDesc );
		srvDesc.Format = texDesc.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		ThrowIfFailed( mDevice->CreateShaderResourceView( mDebugTexture.Get( ), &srvDesc, &mDebugTextureSRV ) );

		ZeroMemoryAndDeclare( D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc );
		rtvDesc.Format = texDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		ThrowIfFailed( mDevice->CreateRenderTargetView( mDebugTexture.Get( ), &rtvDesc, &mDebugTextureRTV ) );

#endif
	};
};


#include "RenderTexture.h"



RenderTexture::RenderTexture( ID3D11Device * Device, ID3D11DeviceContext * Context,
	UINT Width, UINT Height, float camNear, float camFar ) :
	mDevice( Device ),
	mContext( Context ),
	mWidth( float( Width ) ),
	mHeight( float( Height ) ),
	mCamNear( camNear ),
	mCamFar( camFar )
{
	try
	{
		InitViews( );
		mOrthoMatrix = DirectX::XMMatrixOrthographicLH( mWidth, mHeight, mCamNear, mCamFar );
	}
	CATCH;
}


RenderTexture::~RenderTexture( )
{
	mTexture.Reset( );
	mTextureSRV.Reset( );
	mTextureRTV.Reset( );
	mDSView.Reset( );
}


void RenderTexture::InitViews( )
{
	ZeroMemoryAndDeclare( D3D11_TEXTURE2D_DESC, texDesc );
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET | D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width = ( UINT ) mWidth;
	texDesc.Height = ( UINT ) mHeight;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	DX::ThrowIfFailed( mDevice->CreateTexture2D(
		&texDesc, nullptr, &mTexture
		) );


	ZeroMemoryAndDeclare( D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc );
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DX::ThrowIfFailed( mDevice->CreateShaderResourceView(
		mTexture.Get( ), &srvDesc, &mTextureSRV
		) );


	ZeroMemoryAndDeclare( D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc );
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed( mDevice->CreateRenderTargetView(
		mTexture.Get( ), &rtvDesc, &mTextureRTV
		) );


	ZeroVariable( texDesc );
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDesc.Width = ( UINT ) mWidth;
	texDesc.Height = ( UINT ) mHeight;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	
	ID3D11Texture2D * DepthBuffer = nullptr;
	DX::ThrowIfFailed( mDevice->CreateTexture2D( &texDesc, nullptr, &DepthBuffer ) );
	DX::ThrowIfFailed( mDevice->CreateDepthStencilView(
		DepthBuffer, nullptr, &mDSView
		) );

	DepthBuffer->Release( );
}

void RenderTexture::PrepareForRendering( )
{
	mContext->OMSetRenderTargets( 1, mTextureRTV.GetAddressOf( ), mDSView.Get( ) );
}

void RenderTexture::ClearBuffer( )
{
	mContext->ClearRenderTargetView( mTextureRTV.Get( ), mClearColor );
	mContext->ClearDepthStencilView( mDSView.Get( ), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH |
		D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0 );
}
#pragma once

#include "Texture.h"

ALIGN16 class RenderTexture
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mTextureRTV;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSView;

	DirectX::XMMATRIX mOrthoMatrix;

	float mWidth;
	float mHeight;
	float mCamNear;
	float mCamFar;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

	float mClearColor[ 4 ];
public:
	RenderTexture( ID3D11Device * Device, ID3D11DeviceContext * Context,
		UINT Width, UINT Height, float camNear, float camFar );
	~RenderTexture( );
private:
	void InitViews( );
public:
	void PrepareForRendering( );
	void ClearBuffer( );
public:
	inline ID3D11ShaderResourceView* GetTexture( )
	{
		return mTextureSRV.Get( );
	}
public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void * object )
	{
		return _aligned_free( object );
	};
};


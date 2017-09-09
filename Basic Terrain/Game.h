#pragma once

#include "DefaultShader.h"
#include "Model.h"
#include "HRTimer.h"
#include "Square.h"
#include "Font.h"
#include "Text.h"
#include "Input.h"
#include "Camera.h"
#include "C3DShader.h"
#include "Terrain.h"

#define GAME CGame::GetGameInstance( )
#if defined UNICODE
#define ENGINE_NAME L"Apa Borsec"
#else
#define ENGINE_NAME "Apa Borsec"
#endif

ALIGN16 class CGame sealed
{
	static constexpr const float NearZ = 0.1f;
	static constexpr const float FarZ = 100.0f;
	static constexpr const float FOV = ( float ) D3DX_PI / 4.0f;
private:
	HINSTANCE mhInstance;
	HWND mhWnd;

	UINT mWidth;
	UINT mHeight;

	CHRTimer mTimer;
	D3D11_VIEWPORT mFullscreenViewport;
	
	std::unique_ptr<CModel> mTriangle;
	std::unique_ptr<CCamera> mCamera;
	std::unique_ptr<CTerrain> mTerrain;

	std::unique_ptr<CText> mFPSText;

	std::shared_ptr<CInput> mInput;

	std::shared_ptr<CFont> mArial73;
	std::shared_ptr<CFont> mOpenSans32;

	std::shared_ptr<CDefaultShader> mDefaultShader;
	std::shared_ptr<C2DShader> m2DShader;
	std::shared_ptr<C3DShader> m3DShader;

	WCHAR* mGPUDescription;
	
	DirectX::XMMATRIX mOrthoMatrix;

private: // D3D Objects
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mBackbuffer;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSView;

private:
	static CGame* m_GameInstance;

private:
	CGame( HINSTANCE hInstance, bool bFullScreen = false );
	~CGame( );

private:
	void InitWindow( bool bFullscreen );
	void InitD3D( bool bFullscreen );
	void InitShaders( );
	void InitModels( );
	void Init2D( );
	void DeleteWindow( );

	void EnableBackbuffer( );

	void Update( );
	void Render( );

public:
	static bool Initialize( HINSTANCE hInstance, bool bFullScreen = false );
	static CGame* GetGameInstance( );
	static void Shutdown( );

private:
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );

public:
	void Run( );

public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	}
};


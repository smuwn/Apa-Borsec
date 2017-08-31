#pragma once

#include "commonincludes.h"
#include "HRTimer.h"

#define GAME CGame::GetGameInstance( )
#if defined UNICODE
#define ENGINE_NAME L"Apa Borsec"
#else
#define ENGINE_NAME "Apa Borsec"
#endif

class CGame sealed
{

private:
	HINSTANCE mhInstance;
	HWND mhWnd;

	UINT mWidth;
	UINT mHeight;

	CHRTimer mTimer;

	WCHAR* mGPUDescription;
	

private: // D3D Objects
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mBackbuffer;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;

private:
	static CGame* m_GameInstance;

private:
	CGame( HINSTANCE hInstance, bool bFullScreen = false );
	~CGame( );

private:
	void InitWindow( bool bFullscreen );
	void InitD3D( bool bFullscreen );
	void DeleteWindow( );

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
};


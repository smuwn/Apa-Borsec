#include "Game.h"

CGame* CGame::m_GameInstance = nullptr;


CGame::CGame( HINSTANCE hInstance, bool bFullscreen ) :
	mhInstance( hInstance ),
	mWidth( 800 ),
	mHeight( 600 ),
	mOrthoMatrix( DirectX::XMMATRIX( ) )
{
	try
	{
		InitWindow( bFullscreen );
		InitD3D( bFullscreen );
		InitShaders( );
		InitModels( );
		Init2D( );
		mOrthoMatrix = DirectX::XMMatrixOrthographicLH( ( float ) mWidth, ( float ) mHeight, NearZ, FarZ );
	}
	CATCH;
}


CGame::~CGame( )
{
	mInput.reset( );
	mCamera.reset( );
	mDefaultShader.reset( );
	m2DShader.reset( );
	m3DShader.reset( );
	mTriangle.reset( );
	mFPSText.reset( );

	mDevice.Reset( );
	mImmediateContext.Reset( );
	mBackbuffer.Reset( );
	mSwapChain.Reset( );

	DeleteWindow( );
}

void CGame::InitWindow( bool bFullscreen )
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof( WNDCLASSEX );
	wndClass.hbrBackground = ( HBRUSH ) ( GetStockObject( LTGRAY_BRUSH ) );
	wndClass.hInstance = mhInstance;
	wndClass.lpfnWndProc = CGame::WndProc;
	wndClass.lpszClassName = ENGINE_NAME;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	if ( !RegisterClassEx( &wndClass ) )
		throw std::exception( "Couldn't register class to windows" );
	if ( bFullscreen )
	{
		mWidth = GetSystemMetrics( SM_CXSCREEN );
		mHeight = GetSystemMetrics( SM_CYSCREEN );
	}
	mhWnd = CreateWindow(
		ENGINE_NAME, ENGINE_NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, mWidth, mHeight,
		nullptr, nullptr, mhInstance, nullptr
	);
	if ( !mhWnd )
		throw std::exception( "Couldn't create window" );

	UpdateWindow( mhWnd );
	ShowWindow( mhWnd, SW_SHOWNORMAL );
	SetFocus( mhWnd );

	mFullscreenViewport.Width = ( FLOAT ) mWidth;
	mFullscreenViewport.Height = ( FLOAT ) mHeight;
	mFullscreenViewport.TopLeftX = 0;
	mFullscreenViewport.TopLeftY = 0;
	mFullscreenViewport.MinDepth = 0.0f;
	mFullscreenViewport.MaxDepth = 1.0f;
}

void CGame::InitD3D( bool bFullscreen )
{
	IDXGIFactory * Factory;
	DX::ThrowIfFailed( CreateDXGIFactory( __uuidof( IDXGIFactory ),
		reinterpret_cast< void** >( &Factory ) ) );
	IDXGIAdapter * Adapter;
	DX::ThrowIfFailed( Factory->EnumAdapters( 0, &Adapter ) );
	IDXGIOutput * Output;
	DX::ThrowIfFailed( Adapter->EnumOutputs( 0, &Output ) );
	UINT NumModes;
	DX::ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &NumModes, nullptr ) );
	DXGI_MODE_DESC * Modes = new DXGI_MODE_DESC[ NumModes ];
	DX::ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &NumModes, Modes ) );
	DXGI_MODE_DESC FinalMode;
	for ( size_t i = 0; i < NumModes; ++i )
	{
		if ( Modes[ i ].Width == mWidth && Modes[ i ].Height == mHeight )
		{
			FinalMode = DXGI_MODE_DESC( Modes[ i ] );
			break;
		}
	}
	delete[ ] Modes;
	DXGI_ADAPTER_DESC GPU;
	Adapter->GetDesc( &GPU );
	mGPUDescription = GPU.Description;
	ZeroMemoryAndDeclare( DXGI_SWAP_CHAIN_DESC, swapDesc );
	swapDesc.BufferCount = 1;
	swapDesc.BufferDesc = FinalMode;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG::DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.OutputWindow = mhWnd;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = !bFullscreen;

	// MSAA
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	UINT flags = 0;
#if DEBUG || _DEBUG
	flags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	DX::ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, NULL, flags,
			NULL, NULL, D3D11_SDK_VERSION, &swapDesc, &mSwapChain, &mDevice, NULL, &mImmediateContext )
	);

	ID3D11Texture2D * backBufferResource;
	DX::ThrowIfFailed( mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
		reinterpret_cast< void** >( &backBufferResource ) ) );
	DX::ThrowIfFailed( 
		mDevice->CreateRenderTargetView(
			backBufferResource,nullptr, &mBackbuffer
		)
	);

	ID3D11Texture2D * DSViewResource;
	ZeroMemoryAndDeclare( D3D11_TEXTURE2D_DESC, texDesc );
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	texDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	DX::ThrowIfFailed(
		mDevice->CreateTexture2D( &texDesc, nullptr, &DSViewResource )
		);
	DX::ThrowIfFailed(
		mDevice->CreateDepthStencilView( DSViewResource,nullptr,&mDSView )
		);
	DSViewResource->Release( );

	DX::InitStates( mDevice.Get( ) );
	backBufferResource->Release( );
	Factory->Release( );
	Adapter->Release( );
	Output->Release( );
	mInput = std::make_shared<CInput>( );
	mInput->Initialize( mhInstance, mhWnd );
}

void CGame::InitShaders( )
{
	mDefaultShader = std::make_shared<CDefaultShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	m2DShader = std::make_shared<C2DShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	m3DShader = std::make_shared<C3DShader>( mDevice.Get( ), mImmediateContext.Get( ) );
}

void CGame::InitModels( )
{
	mCamera = std::make_unique<CCamera>( mInput, FOV, ( float ) mWidth / ( float ) mHeight, NearZ, FarZ );
	mTriangle = std::make_unique<CModel>( mDevice.Get( ), mImmediateContext.Get( ) );
	mTerrain = std::make_unique<CTerrain>( mDevice.Get( ), mImmediateContext.Get( ), m3DShader, ( LPSTR )"Data/Main.bmp" );
}

void CGame::Init2D( )
{
	mArial73 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/73Arial.fnt" );
	mOpenSans32 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/32OpenSans.fnt" );
	mFPSText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mOpenSans32, mWidth, mHeight );
}

void CGame::Run( )
{
	MSG Message;
	mTimer.Start( );
	while ( true )
	{
		if ( PeekMessage( &Message, nullptr, 0, 0, PM_REMOVE ) )
		{
			if ( Message.message == WM_QUIT )
				break;
			TranslateMessage( &Message );
			DispatchMessage( &Message );
		}
		else
		{
			if ( mInput->isKeyPressed( DIK_ESCAPE ) )
				break;
			if ( mTimer.GetTimeSinceLastStart( ) > 1.0f )
			{
				mTimer.Start( );
			}
			Update( );
			Render( );
		}
	}
}

void CGame::Update( )
{
	mTimer.Frame( );
	mInput->Frame( );
	mCamera->Frame( mTimer.GetFrameTime( ) );
}

void CGame::Render( )
{
	static FLOAT BackColor[ 4 ] = { 0,0,0,0 };
	EnableBackbuffer( );
	mImmediateContext->ClearRenderTargetView( mBackbuffer.Get( ), BackColor );
	mImmediateContext->ClearDepthStencilView( mDSView.Get( ), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0 );

	DirectX::XMMATRIX View, Projection;
	View = mCamera->GetView( );
	Projection = mCamera->GetProjection( );

	mTerrain->Render( View, Projection );

	char buffer[ 500 ] = { 0 };
	sprintf_s( buffer, "FPS: %d", mTimer.GetFPS( ) );

	mFPSText->Render( mOrthoMatrix, buffer, 0, 0,
		DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) );

	mSwapChain->Present( 1, 0 );
}

void CGame::DeleteWindow( )
{
	UnregisterClass( ENGINE_NAME, mhInstance );
	DestroyWindow( mhWnd );
}

void CGame::EnableBackbuffer( )
{
	mImmediateContext->RSSetViewports( 1, &mFullscreenViewport );
	mImmediateContext->OMSetRenderTargets( 1, mBackbuffer.GetAddressOf( ), mDSView.Get( ) );
}

bool CGame::Initialize( HINSTANCE hInstance, bool bFullScreen )
{
	try
	{
		if ( m_GameInstance /*!= nullptr*/ )
			throw std::exception( "Already created" );
		m_GameInstance = new CGame( hInstance, bFullScreen );
	}
	catch ( ... )
	{
		return false;
	}
	return true;
}

CGame * CGame::GetGameInstance( )
{
	return m_GameInstance;
}

void CGame::Shutdown( )
{
	delete m_GameInstance;
}

LRESULT CGame::WndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch ( Message )
	{
	case WM_QUIT:
	DestroyWindow( hWnd );
	break;
	case WM_DESTROY:
	PostQuitMessage( 0 );
	break;
	}
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

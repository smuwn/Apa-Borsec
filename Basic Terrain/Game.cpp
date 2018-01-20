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
		InitTextures( );
		mInput->addSpecialKey( DIK_B );
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
	mDepthShader.reset( );
	mFPSText.reset( );
	mTerrain.reset( );
#if DEBUG || _DEBUG
	mDrawnFacesText.reset( );
	mDebugSquare.reset( );
	mRenderTextureDebug.reset( );
#endif

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
#if defined NO_GPU
	mWidth = 800;
	mHeight = 600;
#endif
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
	bool bFound = false;
	for ( size_t i = 0; i < NumModes; ++i )
	{
		if ( Modes[ i ].Width == mWidth && Modes[ i ].Height == mHeight )
		{
			FinalMode = DXGI_MODE_DESC( Modes[ i ] );
			bFound = true;
			break;
		}
	}
	if ( !bFound )
	{
		FinalMode.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		FinalMode.Width = mWidth;
		FinalMode.Height = mHeight;
		FinalMode.RefreshRate.Denominator = 0;
		FinalMode.RefreshRate.Numerator = 60;
		FinalMode.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		FinalMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
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
	
	D3D_DRIVER_TYPE driver =
#if defined NO_GPU
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_WARP
#else
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE
#endif
		;

	D3D_FEATURE_LEVEL featureLevels[ ] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );
	D3D_FEATURE_LEVEL featureLevel;

	DX::ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain( NULL, driver, NULL, flags,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &mSwapChain, &mDevice, &featureLevel, &mImmediateContext )
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
	Utilities::Create( mDevice.Get( ) );
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
	mLight.Dir = DirectX::XMFLOAT3( 0.0f, -1.0f, 0.0f );
	mLight.Color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	mLight.Ambient = DirectX::XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.0f );
	m3DShader->SetLight( mLight );
	C3DShader::SClippingPlane plane;
	plane.Plane = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ); // Disabled
	m3DShader->SetClippingPlane( plane );
	mLineShader = std::make_shared<LineShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mSkyShader = std::make_shared<SkyShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mSkyPlaneShader = std::make_shared<SkyPlaneShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mDepthShader = std::make_shared<DepthShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	try
	{
		mFireShaders = std::make_shared<CParticleShader>( mDevice.Get( ), mImmediateContext.Get( ) );
		mFireShaders->CreateStreamOutShaders( L"Shaders/FireSOVertexShader.cso", L"Shaders/FireSOGeometryShader.cso" );
		mFireShaders->CreateRenderShaders( L"Shaders/FireRVertexShader.cso", L"Shaders/FireRGeometryShader.cso",
			L"Shaders/FireRPixelShader.cso" );
		mRainShaders = std::make_shared<CParticleShader>( mDevice.Get( ), mImmediateContext.Get( ) );
		mRainShaders->CreateStreamOutShaders( L"Shaders/RainSOVertexShader.cso", L"Shaders/RainSOGeometryShader.cso" );
		mRainShaders->CreateRenderShaders( L"Shaders/RainRVertexShader.cso", L"Shaders/RainRGeometryShader.cso",
			L"Shaders/RainRPixelShader.cso" );
		mFireworksShaders = std::make_shared<CParticleShader>( mDevice.Get( ), mImmediateContext.Get( ) );
		mFireworksShaders->CreateStreamOutShaders( L"Shaders/FireworksSOVertexShader.cso", L"Shaders/FireworksSOGeometryShader.cso" );
		mFireworksShaders->CreateRenderShaders( L"Shaders/FireworksRVertexShader.cso", L"Shaders/FireworksRGeometryShader.cso",
			L"Shaders/FireworksRPixelShader.cso" );
	}
	CATCH;
}

void CGame::InitModels( )
{
	mCamera = std::make_unique<CCamera>( mInput, FOV, ( float ) mWidth / ( float ) mHeight, NearZ, FarZ );
	mTerrain = std::make_shared<CTerrain>( mDevice.Get( ), mImmediateContext.Get( ), m3DShader,
		( LPSTR ) "Data/heightmap.bmp", ( LPSTR ) "Data/heightmap.normals", ( LPSTR ) "Data/HMColor.bmp" );
	mLineManager = std::make_shared<CLineManager>( mDevice.Get( ), mImmediateContext.Get( ), mLineShader);
	mQuadTree = std::make_shared<QuadTree>( mDevice.Get( ), mImmediateContext.Get( ),
		m3DShader, mTerrain, mLineManager );
	GameGlobals::gQuadTrees.push_back( mQuadTree );
	mTerrain.reset( );
	mSkydome = std::make_unique<Skydome>( mDevice.Get( ), mImmediateContext.Get( ),
		mSkyShader, mSkyPlaneShader );
	mFire = std::make_unique<ParticleSystem>( mDevice.Get( ), mImmediateContext.Get( ),
		mFireShaders );
	mFire->SetEmitPos( DirectX::XMFLOAT3( -7.f, 1.f, -45.f ) );
	mRain = std::make_unique<ParticleSystem>( mDevice.Get( ), mImmediateContext.Get( ),
		mRainShaders );
	mFireworks = std::make_unique<ParticleSystem>( mDevice.Get( ), mImmediateContext.Get( ),
		mFireworksShaders );
	mFireworks->SetEmitPos( DirectX::XMFLOAT3( -8.f, 1.0f, -44.f ) );
}

void CGame::Init2D( )
{
	mArial73 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/73Arial.fnt" );
	mOpenSans32 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/32OpenSans.fnt" );
	mFPSText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mOpenSans32, mWidth, mHeight );
#if DEBUG || _DEBUG
	mDrawnFacesText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mOpenSans32, mWidth, mHeight );
	mDebugSquare = std::make_unique<Square>( mDevice.Get( ), mImmediateContext.Get( ), m2DShader,
		mWidth, mHeight, 100, 100 );
	mDebugSquare->TranslateTo( float( mWidth - mDebugSquare->GetWidth( ) ),
		float( mHeight - mDebugSquare->GetHeight( ) ) );
#endif

	mOrthoMatrix = DirectX::XMMatrixOrthographicLH( ( float ) mWidth, ( float ) mHeight, NearZ, FarZ );
}

void CGame::InitTextures( )
{
	try
	{
		mFireTexture = std::make_shared<CTexture>( ( LPWSTR ) L"Data/FireFlare.dds", mDevice.Get( ) );
		mFire->SetTexture( mFireTexture->GetTexture( ) );
		mFireworksTexture = std::make_shared<CTexture>( ( LPWSTR ) L"Data/Firework.jpg", mDevice.Get( ) );
		mFireworks->SetTexture( mFireworksTexture->GetTexture( ) );
	}
	CATCH;
#if DEBUG || _DEBUG
	mRenderTextureDebug = std::make_unique<RenderTexture>( mDevice.Get( ), mImmediateContext.Get( ),
		mWidth, mHeight, NearZ, FarZ );
	mRenderTextureDebug->PrepareForRendering( );
	mRenderTextureDebug->ClearBuffer( );
	mDebugSquare->SetTexture( mRenderTextureDebug->GetTexture( ) );
#endif
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
	mSkydome->Update( mTimer.GetFrameTime( ) );
	mFire->Update( mTimer.GetFrameTime( ) );
	mRain->Update( mTimer.GetFrameTime( ) );
	mRain->SetEmitPos( mCamera->GetCamPos( ) );
	mFireworks->Update( mTimer.GetFrameTime( ) );
#if DEBUG || _DEBUG
	if ( mInput->isSpecialKeyPressed( DIK_B ) )
		bDrawWireframe = bDrawWireframe ? false : true;
#endif
}

void CGame::Render( )
{
	EnableBackbuffer( );
	ClearBackbuffer( );
	C3DShader::SClippingPlane plane;
	plane.Plane = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	m3DShader->SetClippingPlane( plane );


	DirectX::XMMATRIX View, ReflectView, Projection;
	DirectX::XMFLOAT3 CamPos, ReflectedCamPos;
	View = mCamera->GetView( );
	ReflectView = mCamera->GetReflectView( );
	Projection = mCamera->GetProjection( );
	CamPos = mCamera->GetCamPos( );
	ReflectedCamPos = mCamera->GetReflectedCamPos( );
	
	FrustumCulling::ViewFrustum Frustum = FrustumCulling::ConstructFrustum( View, Projection );

#if DEBUG || _DEBUG
	mLineManager->Begin( );
	for ( auto & iter : GameGlobals::gQuadTrees )
		iter->RenderLines( );
	mLineManager->End( );
	mLineManager->Render( View, Projection );
#endif
	// Render Scene
	int Drawn = 0;
	for ( auto & iter : GameGlobals::gQuadTrees )
	{
		iter->Render( View, Projection, Frustum, Drawn, CamPos.y, bDrawWireframe );
	}

	mSkydome->Render( View, Projection, CamPos );

	EnableBackbuffer( );

	// Render Particles
	mImmediateContext->OMSetBlendState( DX::TransparencyBlend.Get( ), nullptr, 0xffffffff );
	mFire->Render( mCamera.get( ) );
	mFireworks->Render( mCamera.get( ) );
	mImmediateContext->OMSetBlendState( nullptr, nullptr, 0xffffffff );

	mRain->Render( mCamera.get( ) );


	char buffer[ 500 ] = { 0 };
	sprintf_s( buffer, "FPS: %d", mTimer.GetFPS( ) );
	mFPSText->Render( mOrthoMatrix, buffer, 0, 0,
		DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) );

#if DEBUG || _DEBUG
	mDebugSquare->Render( mOrthoMatrix );

	sprintf_s( buffer, "Drawn faces: %d", Drawn );
	mDrawnFacesText->Render( mOrthoMatrix, buffer,
		0, 33 );
#endif


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

void CGame::ClearBackbuffer( )
{
	static FLOAT BackColor[ 4 ] = { 0,0,0,0 };
	mImmediateContext->ClearRenderTargetView( mBackbuffer.Get( ), BackColor );
	mImmediateContext->ClearDepthStencilView( mDSView.Get( ), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH, 1.0f, 0 );
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

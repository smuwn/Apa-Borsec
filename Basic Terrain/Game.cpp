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
		Precompute();
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
	ThrowIfFailed( CreateDXGIFactory( __uuidof( IDXGIFactory ),
		reinterpret_cast< void** >( &Factory ) ) );
	IDXGIAdapter * Adapter;
	ThrowIfFailed( Factory->EnumAdapters( 0, &Adapter ) );
	IDXGIOutput * Output;
	ThrowIfFailed( Adapter->EnumOutputs( 0, &Output ) );
	UINT NumModes;
	ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &NumModes, nullptr ) );
	DXGI_MODE_DESC * Modes = new DXGI_MODE_DESC[ NumModes ];
	ThrowIfFailed( Output->GetDisplayModeList( DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
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

	ThrowIfFailed(
		D3D11CreateDeviceAndSwapChain( NULL, driver, NULL, flags,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &mSwapChain, &mDevice, &featureLevel, &mImmediateContext )
	);

#if DEBUG || _DEBUG

	Microsoft::WRL::ComPtr<ID3D11Debug> debugObject;
	HRESULT hr = mDevice.As<ID3D11Debug>( &debugObject );
	if ( SUCCEEDED( hr ) )
	{
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> debugQueue;
		hr = debugObject.As<ID3D11InfoQueue>( &debugQueue );
		if ( SUCCEEDED( hr ) )
		{
			D3D11_MESSAGE_ID hiddenMessages[ ] = 
			{
				D3D11_MESSAGE_ID::D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
			};
			ZeroMemoryAndDeclare( D3D11_INFO_QUEUE_FILTER, hideFilter );
			hideFilter.DenyList.NumIDs = ARRAYSIZE( hiddenMessages );
			hideFilter.DenyList.pIDList = hiddenMessages;
			debugQueue->AddStorageFilterEntries( &hideFilter );
		}
	}
	if ( FAILED( hr ) )
		DX::OutputVDebugString( L"Can't create a debug object.\n" );

#endif

	ID3D11Texture2D * backBufferResource;
	ThrowIfFailed( mSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
		reinterpret_cast< void** >( &backBufferResource ) ) );
	ThrowIfFailed( 
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
	ThrowIfFailed(
		mDevice->CreateTexture2D( &texDesc, nullptr, &DSViewResource )
		);
	ThrowIfFailed(
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

void CGame::Precompute()
{
	mComputeFFT = std::make_unique<PrecomputeFFT>(mDevice.Get(), mImmediateContext.Get());
	mComputeFFT->Compute();
	mTimeComputeFFT = std::make_unique<TimeDependentFFT>(mDevice.Get(), mImmediateContext.Get());
	mTimeComputeFFT->SetComponents(mComputeFFT->GetH0(), mComputeFFT->GetH0Minus());
	mTwiddleIndicesFFT = std::make_unique<ComputeTwiddleIndices>(mDevice.Get(), mImmediateContext.Get());
	mTwiddleIndicesFFT->Compute();
}

void CGame::InitShaders( )
{
	mDefaultShader = std::make_shared<CDefaultShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	m2DShader = std::make_shared<C2DShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	m3DShader = std::make_shared<C3DShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	C3DShader::SClippingPlane plane;
	plane.Plane = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f ); // Disabled
	m3DShader->SetClippingPlane( plane );
	mLineShader = std::make_shared<LineShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mSkyShader = std::make_shared<SkyShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mSkyPlaneShader = std::make_shared<SkyPlaneShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mDepthShader = std::make_shared<DepthShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mProjectiveShaders = std::make_shared<ProjectiveTexturingShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mShadowMapShader = std::make_shared<ShadowMapShader>( mDevice.Get( ), mImmediateContext.Get( ) );
	mModelShader = std::make_shared<ModelShader>( mDevice.Get( ), mImmediateContext.Get( ) );
}

void CGame::InitModels( )
{
	mCamera = std::make_unique<CCamera>( mInput, FOV, ( float ) mWidth / ( float ) mHeight, NearZ, FarZ );
	mTerrain = std::make_shared<CTerrain>( mDevice.Get( ), mImmediateContext.Get( ), m3DShader,
		( LPSTR ) "Data/heightmap.bmp", ( LPSTR ) "Data/heightmap.normals", ( LPSTR ) "Data/HMColor.bmp" );
	mLineManager = std::make_shared<CLineManager>( mDevice.Get( ), mImmediateContext.Get( ), mLineShader);
	mQuadTree = std::make_shared<QuadTree>( mDevice.Get( ), mImmediateContext.Get( ),
		m3DShader, mShadowMapShader, mTerrain, mLineManager );
	GameGlobals::gQuadTrees.push_back( mQuadTree );
	mTerrain.reset( );
	mSkydome = std::make_unique<Skydome>( mDevice.Get( ), mImmediateContext.Get( ),
		mSkyShader, mSkyPlaneShader );

	mModel = std::make_unique<CModel>( mDevice.Get( ), mImmediateContext.Get( ) );
	mModel->Identity( );
	mModel->Translate( 7.05f, -4.0f, -30.51f );
	mModel->SetShader( mModelShader );

	mShadowMap = std::make_unique<BuildShadowMap<DX::Projections::PerspectiveProjection>>( 
		mDevice.Get( ), mImmediateContext.Get( ), 2048.0f
		);
	mShadowMap->Initialize( FOV, float( mWidth ) / float( mHeight ), NearZ, FarZ );
	mShadowMap->SetPosition(DirectX::XMVectorSet(-157.f, 90.f, -156.f, 1.0f));
	mShadowMap->SetDirection(DirectX::XMVectorSet(0.6f, -.5f, 0.6f, 0.0f));
	mShadowMap->Construct( );
	m3DShader->SetShadowMap( mShadowMap->GetShadowMapSRV( ) );
	mModelShader->SetShadowMap( mShadowMap->GetShadowMapSRV( ) );
	mCamera->SetPosition( DirectX::XMVectorSet( -3.46f, 1.16f, -55.21f, 1.0f ) );
	mCamera->SetDirection( DirectX::XMVectorSet( 0.58f, -0.11f, 0.86f, 0.0f ) );
	ModelShader::SLightPS lightPSInfo;
	lightPSInfo.Color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	DirectX::XMStoreFloat3( &lightPSInfo.Position, mShadowMap->GetPosition( ) );
	ModelShader::SLightVS lightVSInfo;
	lightVSInfo.View = DirectX::XMMatrixTranspose( mShadowMap->GetView( ) );
	lightVSInfo.Projection = DirectX::XMMatrixTranspose( mShadowMap->GetProjection( ) );
	mModelShader->SetLight( lightVSInfo, lightPSInfo );
	mLightVS.ViewProjection = DirectX::XMMatrixTranspose( mShadowMap->GetView( ) * mShadowMap->GetProjection( ) );
	mLightPS.Ambient = DirectX::XMFLOAT4( 0.1f, 0.1f, 0.1f, 0.1f );
	mLightPS.Color = lightPSInfo.Color;
	mLightPS.Pos = lightPSInfo.Position;
	m3DShader->SetLight( mLightVS, mLightPS );
}

void CGame::Init2D( )
{
	mArial73 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/73Arial.fnt" );
	mOpenSans32 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/32OpenSans.fnt" );
	mKristen16 = std::make_shared<CFont>( mDevice.Get( ), mImmediateContext.Get( ),
		( LPWSTR ) L"Fonts/16Kristen.fnt" );
	mFPSText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mOpenSans32, mWidth, mHeight );
#if DEBUG || _DEBUG
	mDebugSquare = std::make_unique<Square>( mDevice.Get( ), mImmediateContext.Get( ), m2DShader,
		mWidth, mHeight, 100, 100 );
	mDebugSquare->TranslateTo( float( mWidth - mDebugSquare->GetWidth( ) ),
		float( mHeight - mDebugSquare->GetHeight( ) ) );
	mCamPosText = std::make_unique<CText>( mDevice.Get( ), mImmediateContext.Get( ),
		m2DShader, mKristen16, mWidth, mHeight );
#endif

	mH0Square = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mH0Square->TranslateTo(0, 50);
	mH0Square->SetTexture(mComputeFFT->GetH0());
	mMinusH0Square = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mMinusH0Square->TranslateTo(205, 50);
	mMinusH0Square->SetTexture(mComputeFFT->GetH0Minus());
	mTwiddleSquare = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mTwiddleSquare->TranslateTo(410, 50);
	mTwiddleSquare->SetTexture(mTwiddleIndicesFFT->GetTwiddleTexture());

	mDXSquare = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mDXSquare->TranslateTo(0, 255);
	mDXSquare->SetTexture(mTimeComputeFFT->GetTextureSRVDX());

	mDYSquare = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mDYSquare->TranslateTo(205, 255);
	mDYSquare->SetTexture(mTimeComputeFFT->GetTextureSRVDY());

	mDZSquare = std::make_unique<Square>(mDevice.Get(), mImmediateContext.Get(), m2DShader,
		mWidth, mHeight, 200, 200);
	mDZSquare->TranslateTo(410, 255);
	mDZSquare->SetTexture(mTimeComputeFFT->GetTextureSRVDZ());

	mOrthoMatrix = DirectX::XMMatrixOrthographicLH( ( float ) mWidth, ( float ) mHeight, NearZ, FarZ );
}

void CGame::InitTextures( )
{
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

	/// Rotate the light
	//DirectX::XMVECTOR lightDirection = mShadowMap->GetDirection( );
	//lightDirection = DirectX::XMVector3TransformCoord( lightDirection, DirectX::XMMatrixRotationY( 0.002f ) );
	//mShadowMap->SetDirection( lightDirection );
	//mShadowMap->Construct( );
	//ModelShader::SLightPS lightPSInfo;
	//lightPSInfo.Color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	//DirectX::XMStoreFloat3( &lightPSInfo.Position, mShadowMap->GetPosition( ) );
	//ModelShader::SLightVS lightVSInfo;
	//lightVSInfo.View = DirectX::XMMatrixTranspose( mShadowMap->GetView( ) );
	//lightVSInfo.Projection = DirectX::XMMatrixTranspose( mShadowMap->GetProjection( ) );
	//mModelShader->SetLight( lightVSInfo, lightPSInfo );
	//mLightVS.ViewProjection = DirectX::XMMatrixTranspose( mShadowMap->GetView( ) * mShadowMap->GetProjection( ) );
	//mLightPS.Ambient = DirectX::XMFLOAT4( 0.1f, 0.1f, 0.1f, 0.1f );
	//mLightPS.Color = lightPSInfo.Color;
	//mLightPS.Pos = lightPSInfo.Position;
	//m3DShader->SetLight( mLightVS, mLightPS );

	mCamera->Frame( mTimer.GetFrameTime( ) );
	mSkydome->Update( mTimer.GetFrameTime( ) );
#if DEBUG || _DEBUG
	if ( mInput->isSpecialKeyPressed( DIK_B ) )
		bDrawWireframe = bDrawWireframe ? false : true;
	if ( mInput->isKeyPressed( DIK_NUMPAD4 ) )
		mModel->Translate( -1.0f, 0.0f, 0.0f );
#endif
	//mComputeFFT->Compute();
	//mTwiddleIndicesFFT->Compute();
	mTimeComputeFFT->Compute(mTimer.GetTotalTime(), mComputeFFT->GetInfo());
}

void CGame::Render( )
{
	int Drawn = 0;
	mShadowMap->PrepareForRendering( );
	mShadowMap->ClearBuffer( );
	DirectX::XMMATRIX shadowView = mShadowMap->GetView( );
	DirectX::XMMATRIX shadowProjection = mShadowMap->GetProjection( );
	float projectorY = DirectX::XMVectorGetY( mShadowMap->GetPosition( ) );

	FrustumCulling::ViewFrustum shadowFrustum = FrustumCulling::ConstructFrustum( shadowView, shadowProjection );

	mModel->Render( );
	DirectX::XMMATRIX modelWorld = mModel->GetWorld( );
	mShadowMapShader->RenderIndices( mModel->GetIndexCount( ), modelWorld * shadowView * shadowProjection, FALSE );
	for ( auto & iter : GameGlobals::gQuadTrees )
	{
		iter->RenderShadowMap( );
		iter->Render( shadowView, shadowProjection, shadowFrustum, Drawn, projectorY );
		iter->Render3D( );
	}
	

	EnableBackbuffer( );
	ClearBackbuffer( );
	C3DShader::SClippingPlane plane;
	plane.Plane = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	m3DShader->SetClippingPlane( plane );


	DirectX::XMMATRIX View, ReflectView, Projection;
	DirectX::XMFLOAT3 CamPos, ReflectedCamPos, CamDir;
	View = mCamera->GetView( );
	ReflectView = mCamera->GetReflectView( );
	Projection = mCamera->GetProjection( );
	CamPos = mCamera->GetCamPos( );
	CamDir = mCamera->GetCamDir( );
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
	mModel->Render( View, Projection );

	for ( auto & iter : GameGlobals::gQuadTrees )
	{
		iter->Render( View, Projection, Frustum, Drawn, CamPos.y, bDrawWireframe );
	}

	mSkydome->Render( View, Projection, CamPos );
	
	EnableBackbuffer( );

	mH0Square->Render(mOrthoMatrix);
	mMinusH0Square->Render(mOrthoMatrix);
	mTwiddleSquare->Render(mOrthoMatrix);
	mDXSquare->Render(mOrthoMatrix);
	mDYSquare->Render(mOrthoMatrix);
	mDZSquare->Render(mOrthoMatrix);

	char buffer[ 500 ] = { 0 };
	sprintf_s( buffer, "FPS: %d", mTimer.GetFPS( ) );
	mFPSText->Render( mOrthoMatrix, buffer, 0, 0,
		DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) );

#if DEBUG || _DEBUG
	sprintf_s( buffer, "Cam pos: (%.2f,%.2f,%.2f)", CamPos.x, CamPos.y, CamPos.z );

	mCamPosText->Render( mOrthoMatrix, buffer, 0, float( mHeight - 32 ),
		DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) );

	mDebugSquare->Render( mOrthoMatrix );
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
	mImmediateContext->RSSetState( DX::DefaultRS.Get( ) );
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

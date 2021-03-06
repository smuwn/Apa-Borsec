#include "Camera.h"


const DirectX::XMVECTOR CCamera::Forward = DirectX::XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
const DirectX::XMVECTOR CCamera::Right = DirectX::XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
const DirectX::XMVECTOR CCamera::Up = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
const DirectX::XMVECTOR CCamera::DefaultPosition = DirectX::XMVectorSet( 1.0f, 1.0f, 1.0f, 1.0f );
const DirectX::XMMATRIX CCamera::DefaultView = DirectX::XMMatrixLookToLH( DefaultPosition, Forward, Up );



CCamera::CCamera( std::shared_ptr<CInput> Input, float FOV, float HByW, float NearZ, float FarZ ) :
	mInput( Input )
{
	mProjection = DirectX::XMMatrixPerspectiveFovLH( FOV, HByW, NearZ, FarZ );
}


CCamera::~CCamera( )
{
	mInput.reset( );
}


void CCamera::Frame( float frameTime )
{
#if defined DEFINED_USE_ROTATION_ACCELERATION
	mYRotationAcceleration += mInput->GetHorizontalMouseMove( ) * 0.001f;
	mZRotationAcceleration += mInput->GetVerticalMouseMove( ) * 0.001f;

	mYaw += ( mYRotationAcceleration * ( 1.0f - RotationFriction ) );
	mYRotationAcceleration *= ( 1.0f - RotationFriction );

	mPitch += ( mZRotationAcceleration * ( 1.0f - RotationFriction ) );
	mZRotationAcceleration *= ( 1.0f - RotationFriction );

#else

	mYaw += mInput->GetHorizontalMouseMove( ) * 0.001f;
	mPitch += mInput->GetVerticalMouseMove( ) * 0.001f;

#endif
	if ( mInput->isKeyPressed( DIK_LSHIFT ) )
		frameTime *= 10;
	if ( mInput->isKeyPressed( DIK_W ))
		mForwardAcceleration += CamSpeed * frameTime;
	if ( mInput->isKeyPressed( DIK_S ) )
		mForwardAcceleration -= CamSpeed * frameTime;
	if ( mInput->isKeyPressed( DIK_D ) )
		mRightAcceleration += CamSpeed * frameTime;
	if ( mInput->isKeyPressed( DIK_A ) )
		mRightAcceleration -= CamSpeed * frameTime;
	DX::clamp( mForwardAcceleration, -MaxCamSpeed, MaxCamSpeed );
	DX::clamp( mRightAcceleration, -MaxCamSpeed, MaxCamSpeed );

	DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw( mPitch, mYaw, 0.0f );
	mDirection = DirectX::XMVector3TransformCoord( Forward, Rotation );
	mRight = DirectX::XMVector3TransformCoord( Right, Rotation );
	mUp = DirectX::XMVector3Cross( mDirection, mRight );



#if DEBUG || _DEBUG
	mPosition = DirectX::XMVectorAdd( mPosition, DirectX::XMVectorScale( mDirection, mForwardAcceleration ) );
	mPosition = DirectX::XMVectorAdd( mPosition, DirectX::XMVectorScale( mRight, mRightAcceleration ) );
#else
	DirectX::XMVECTOR FlattenDirection = mDirection;
	FlattenDirection = DirectX::XMVectorSetY( FlattenDirection, 0.0f );
	FlattenDirection = DirectX::XMVector3Normalize( FlattenDirection );
	mPosition = DirectX::XMVectorAdd( mPosition, DirectX::XMVectorScale( FlattenDirection, mForwardAcceleration ) );
	mPosition = DirectX::XMVectorAdd( mPosition, DirectX::XMVectorScale( mRight, mRightAcceleration ) );
	float Y= DirectX::XMVectorGetY( mPosition );
	float X = DirectX::XMVectorGetX( mPosition );
	float Z = DirectX::XMVectorGetZ( mPosition );
	float height;

	bool bFound = false;
	for ( auto & iter : GameGlobals::gQuadTrees )
	{
		if ( iter->GetHeightAt( X, Z, height ) )
		{
			bFound = true;
			break;
		}
	}
	if ( bFound )
	{
		mPosition = DirectX::XMVectorSetY( mPosition, height + CameraOffset );
	}
	else // Free falling
	{
		mPosition = DirectX::XMVectorSetY( mPosition, Y - CameraFallingSpeed * frameTime );
	}
#endif
	mReflectedPosition = DirectX::XMVector3Reflect( mPosition, Up );
	mReflectedDirection = DirectX::XMVector3Reflect( mDirection, Up );
	mView = DirectX::XMMatrixLookToLH( mPosition, mDirection, mUp );
	mReflectView = DirectX::XMMatrixLookToLH( mReflectedPosition, mReflectedDirection, Up );

	mForwardAcceleration *= ( 1.0f - Friction );
	mRightAcceleration *= ( 1.0f - Friction );
}
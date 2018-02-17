#pragma once

#include "commonincludes.h"
#include "Input.h"
#include "GameGlobals.h"

#define DEFINED_USE_ROTATION_ACCELERATION

ALIGN16 class CCamera sealed
{
public:
	static const DirectX::XMVECTOR Forward;
	static const DirectX::XMVECTOR Right;
	static const DirectX::XMVECTOR Up;
	static const DirectX::XMVECTOR DefaultPosition;
	static const DirectX::XMMATRIX DefaultView;
	static constexpr const float CameraFallingSpeed = 9.81f;
	static constexpr const float CameraOffset = 5.0f;
	static constexpr const float Friction = 0.1f;
	static constexpr const float CamSpeed = 0.3f;
	static constexpr const float MaxCamSpeed = 10;
#if defined DEFINED_USE_ROTATION_ACCELERATION
	static constexpr const float RotationFriction = 0.2f;
#endif
private:
	std::shared_ptr<CInput> mInput;

	DirectX::XMVECTOR mReflectedDirection = Forward;
	DirectX::XMVECTOR mDirection = Forward;
	DirectX::XMVECTOR mRight = Right;
	DirectX::XMVECTOR mUp = Up;
	DirectX::XMVECTOR mPosition = DefaultPosition;
	DirectX::XMVECTOR mReflectedPosition = DefaultPosition;

	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mReflectView;
	DirectX::XMMATRIX mProjection;

	float mForwardAcceleration = 0.0f;
	float mRightAcceleration = 0.0f;

#if	defined DEFINED_USE_ROTATION_ACCELERATION
	float mYRotationAcceleration = 0.0f;
	float mZRotationAcceleration = 0.0f;
#endif

	float mYaw = 0;
	float mPitch = 0;
	float mRoll = 0;

public:
	CCamera( std::shared_ptr<CInput> Input, float FOV, float HByW, float NearZ, float FarZ );
	~CCamera( );
public:
	void Frame( float frametime );
public:
	inline DirectX::XMMATRIX& GetView( ) { return mView; };
	inline DirectX::XMMATRIX& GetReflectView( ) { return mReflectView; };
	inline DirectX::XMMATRIX& GetProjection( ) { return mProjection; };
	inline DirectX::XMFLOAT3 GetCamPos( )
	{ DirectX::XMFLOAT3 Pos; DirectX::XMStoreFloat3( &Pos, mPosition ); return Pos; };
	inline DirectX::XMFLOAT3 GetReflectedCamPos( )
	{ DirectX::XMFLOAT3 Pos; DirectX::XMStoreFloat3( &Pos, mReflectedPosition ); return Pos; };
	inline DirectX::XMFLOAT3 GetCamDir( )
	{ DirectX::XMFLOAT3 Dir; DirectX::XMStoreFloat3( &Dir, mDirection ); return Dir; };
	inline DirectX::XMFLOAT3 GetCamRotation( )
	{ return DirectX::XMFLOAT3( mPitch, mYaw, mRoll ); };
	inline void SetPosition( DirectX::FXMVECTOR& position )
	{ mPosition = position; };
	inline void SetDirection( DirectX::FXMVECTOR& direction )
	{ mDirection = direction; };
public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size,16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	};
};


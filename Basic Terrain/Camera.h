#pragma once

#include "commonincludes.h"
#include "Input.h"
#if !(DEBUG || _DEBUG)
#include "GameGlobals.h"
#endif

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
	static constexpr const float CameraOffset = 2.0f;
	static constexpr const float Friction = 0.1f;
	static constexpr const float CamSpeed = 0.3f;
	static constexpr const float MaxCamSpeed = 10;
#if defined DEFINED_USE_ROTATION_ACCELERATION
	static constexpr const float RotationFriction = 0.2f;
#endif
private:
	std::shared_ptr<CInput> mInput;

	DirectX::XMVECTOR mDirection = Forward;
	DirectX::XMVECTOR mRight = Right;
	DirectX::XMVECTOR mUp = Up;
	DirectX::XMVECTOR mPosition = DefaultPosition;

	DirectX::XMMATRIX mView;
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
	inline DirectX::XMMATRIX& GetProjection( ) { return mProjection; };
	inline DirectX::XMFLOAT3 GetCamPos( )
	{ DirectX::XMFLOAT3 Pos; DirectX::XMStoreFloat3( &Pos, mPosition ); return Pos; };
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


#pragma once

#include "commonincludes.h"
#include "Input.h"

#define DEFINED_USE_ROTATION_ACCELERATION

ALIGN16 class CCamera
{
public:
	static const DirectX::XMVECTOR Forward;
	static const DirectX::XMVECTOR Right;
	static const DirectX::XMVECTOR Up;
	static const DirectX::XMVECTOR DefaultPosition;
	static const DirectX::XMMATRIX DefaultView;
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


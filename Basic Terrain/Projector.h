#pragma once

#include "ProjectiveTexturingShader.h"


ALIGN16 class Projector
{
	DirectX::XMMATRIX mProjectorView;
	DirectX::XMMATRIX mProjectorProjection;

	DirectX::XMVECTOR mPosition;
	DirectX::XMVECTOR mDirection;
	DirectX::XMVECTOR mUp;

	float mFieldsOfView;
	float mHByW;
	float mNearZ;
	float mFarZ;

public:
	Projector( float FOV, float HByW, float nearZ, float farZ )
		: mFieldsOfView( FOV ), mHByW( HByW ), mNearZ( nearZ ), mFarZ( farZ ) {};
	~Projector( ) = default;
public:
	DirectX::XMMATRIX& GetView( ) { return mProjectorView; };
	DirectX::XMMATRIX& GetProjection( ) { return mProjectorProjection; };

	DirectX::XMVECTOR& GetPosition( ) { return mPosition; };
	DirectX::XMVECTOR& GetDirection( ) { return mDirection; };

	void SetPosition( DirectX::XMVECTOR& position )
	{
		mPosition = position;
	}

	void SetDirection( DirectX::XMVECTOR& direction )
	{
		mDirection = direction;
	}


	void Construct( )
	{
		mProjectorView = DirectX::XMMatrixLookToLH( mPosition, mDirection, mUp );
		mProjectorProjection = DirectX::XMMatrixPerspectiveFovLH(
			mFieldsOfView, mHByW, mNearZ, mFarZ
		);
	}
public:
	inline void* operator new ( size_t size )
	{
		return _aligned_malloc( size, 16 );
	};
	inline void operator delete ( void* object )
	{
		_aligned_free( object );
	};
};


#pragma once

#include "ProjectiveTexturingShader.h"


template<class ProjectionType>
class Projector
{
	static_assert(
		( std::is_same<ProjectionType, DX::Projections::PerspectiveProjection>::value ||
			std::is_same<ProjectionType, DX::Projections::OrtographicProjection>::value ), 
		"ProjectionType for a projector must be DX::Projections::PerspectiveProjection OR DX::Projections::OrtographicProjection"
		);
	template<bool Test, class T>
	using resolvedType = typename std::enable_if<Test, T>::type;
protected:
	DirectX::XMMATRIX mProjectorView;
	DirectX::XMMATRIX mProjectorProjection;

	DirectX::XMVECTOR mPosition;
	DirectX::XMVECTOR mDirection;
	DirectX::XMVECTOR mUp;

	std::function<void( )> mConstructFunction;
	
	float mFieldsOfView;
	float mHByW;
	float mNearZ;
	float mFarZ;

	float mWidth;
	float mHeight;

public:
	Projector( ) = default;
	~Projector( ) = default;
public:
	/// <summary>This function will be used to initialize a Projector with Perspective Projection</summary>
	template<class Q = ProjectionType>
	resolvedType<std::is_same<Q, DX::Projections::PerspectiveProjection>::value, void>
		Initialize( float FOV, float HbyW, float nearZ, float farZ )
	{
		mFieldsOfView = FOV;
		mHByW = HbyW;
		mNearZ = nearZ;
		mFarZ = farZ;
		mConstructFunction = std::bind( &Projector<DX::Projections::PerspectiveProjection>::ConstructPerspective, this );
	}
	/// <summary>This function will be used to initialize a Projector with Ortographic Projection</summary>
	template <class Q = ProjectionType>
	resolvedType<std::is_same<Q, DX::Projections::OrtographicProjection>::value, void>
		Initialize( float Width, float Height, float nearZ, float farZ )
	{
		mWidth = Width;
		mHeight = Height;
		mNearZ = nearZ;
		mFarZ = farZ;
		mConstructFunction = std::bind( &Projector<DX::Projections::OrtographicProjection>::ConstructOrtographic, this );
	}
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
		mConstructFunction( );
	}
private:
	void ConstructPerspective( )
	{
		mProjectorView = DirectX::XMMatrixLookToLH( mPosition, mDirection, mUp );
		mProjectorProjection = DirectX::XMMatrixPerspectiveFovLH(
			mFieldsOfView, mHByW, mNearZ, mFarZ
		);
	}
	void ConstructOrtographic( )
	{
		mProjectorView = DirectX::XMMatrixLookToLH( mPosition, mDirection, mUp );
		mProjectorProjection = DirectX::XMMatrixOrthographicLH(
			mWidth, mHeight, mNearZ, mFarZ
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


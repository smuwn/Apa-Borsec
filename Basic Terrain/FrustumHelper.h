#pragma once

#include <DirectXMath.h>
#include <array>

namespace FrustumCulling
{
	typedef std::array<DirectX::XMFLOAT4, 6> ViewFrustum;
	inline ViewFrustum ConstructFrustum( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection )
	{
		using namespace DirectX;
		std::array<DirectX::XMFLOAT4, 6> Planes;
		XMMATRIX ViewProj = View * Projection;
		XMFLOAT4X4 ViewProjMat;
		DirectX::XMStoreFloat4x4( &ViewProjMat, ViewProj );
		XMFLOAT4 Plane;
		// Near plane
		Plane.x = ViewProjMat._13;
		Plane.y = ViewProjMat._23;
		Plane.z = ViewProjMat._33;
		Plane.w = ViewProjMat._43;
		Planes[ 0 ] = Plane;
		// Far plane
		Plane.x = ViewProjMat._14 - ViewProjMat._13;
		Plane.y = ViewProjMat._24 - ViewProjMat._23;
		Plane.z = ViewProjMat._34 - ViewProjMat._33;
		Plane.w = ViewProjMat._44 - ViewProjMat._43;
		Planes[ 1 ] = Plane;
		// Left plane
		Plane.x = ViewProjMat._14 + ViewProjMat._11;
		Plane.y = ViewProjMat._24 + ViewProjMat._21;
		Plane.z = ViewProjMat._34 + ViewProjMat._31;
		Plane.w = ViewProjMat._44 + ViewProjMat._41;
		Planes[ 2 ] = Plane;
		// Right plane
		Plane.x = ViewProjMat._14 - ViewProjMat._11;
		Plane.y = ViewProjMat._24 - ViewProjMat._21;
		Plane.z = ViewProjMat._34 - ViewProjMat._31;
		Plane.w = ViewProjMat._44 - ViewProjMat._41;
		Planes[ 3 ] = Plane;
		// Top plane
		Plane.x = ViewProjMat._14 - ViewProjMat._12;
		Plane.y = ViewProjMat._24 - ViewProjMat._22;
		Plane.z = ViewProjMat._34 - ViewProjMat._32;
		Plane.w = ViewProjMat._44 - ViewProjMat._42;
		Planes[ 4 ] = Plane;
		// Bottom plane
		Plane.x = ViewProjMat._14 + ViewProjMat._12;
		Plane.y = ViewProjMat._24 + ViewProjMat._22;
		Plane.z = ViewProjMat._34 + ViewProjMat._32;
		Plane.w = ViewProjMat._44 + ViewProjMat._42;
		Planes[ 5 ] = Plane;

		for ( size_t i = 0; i < Planes.size( ); ++i )
		{
			float length = sqrt( ( Planes[ i ].x * Planes[ i ].x )
				+ ( Planes[ i ].y * Planes[ i ].y )
				+ ( Planes[ i ].z * Planes[ i ].z ) );
			Planes[ i ].x /= length;
			Planes[ i ].y /= length;
			Planes[ i ].z /= length;
			Planes[ i ].w /= length;
		}
		return Planes;
	}
	inline bool isPointInFrustum( float x, float y, float z,
		ViewFrustum const& Frustum )
	{
		using namespace DirectX;
		for ( size_t i = 0; i < Frustum.size( ); ++i )
		{
			XMVECTOR planeNormal = XMVectorSet( Frustum[ i ].x, Frustum[ i ].y, Frustum[ i ].z, 0.0f );
			float planeConstant = Frustum[ i ].w;
			XMVECTOR Point = XMVectorSet( x, y, z, 1.0f );
			if ( XMVectorGetX( XMVector3Dot( planeNormal, Point ) ) + planeConstant < 0.0f )
				return false;
		}
		return true;
	}
	inline bool isAABBInFrustum( float minX, float minY, float minZ,
		float maxX, float maxY, float maxZ,
		ViewFrustum const& Frustum )
	{
		if ( isPointInFrustum( minX, minY, minZ, Frustum ) )
			return true;
		if ( isPointInFrustum( minX, minY, maxZ, Frustum ) )
			return true;
		if ( isPointInFrustum( minX, maxY, minZ, Frustum ) )
			return true;
		if ( isPointInFrustum( maxX, minY, minZ, Frustum ) )
			return true;
		if ( isPointInFrustum( maxX, maxY, minZ, Frustum ) )
			return true;
		if ( isPointInFrustum( maxX, minY, maxZ, Frustum ) )
			return true;
		if ( isPointInFrustum( minX, maxY, maxZ, Frustum ) )
			return true;
		if ( isPointInFrustum( maxX, maxY, maxZ, Frustum ) )
			return true;
		return false;
	}
	inline bool isSphereInFrustum( DirectX::XMFLOAT3 Center, float radius,
		ViewFrustum const& Frustum )
	{
		using namespace DirectX;
		for ( int i = 0; i < 6; ++i )
		{
			XMVECTOR Plane = XMLoadFloat4( &Frustum[ i ] );
			if ( XMVectorGetX(
				XMPlaneDot( Plane, DirectX::XMVectorSet( Center.x, Center.y, Center.z, 1.0f ) )
			) <= -radius )
				return false;
		}
		return true;
	}
	inline bool isRectangleInFrustum( DirectX::XMFLOAT3 Center,
		float sizex, float sizey, float sizez, ViewFrustum const& Frustum )
	{
		using namespace DirectX;
		XMVECTOR p;
		XMFLOAT3 a( Center.x, Center.y, Center.z );

		p = XMLoadFloat3( &a );

		BOOL test = TRUE;
		for ( INT i = 0; i < 6; i++ )
		{
			XMVECTOR plane = XMLoadFloat4( &Frustum[ i ] );
			XMVECTOR axisvert = XMVectorSet( ( Frustum[ i ].x < 0.0f ) ? XMVectorGetX( p ) : XMVectorGetX( p ) + sizex,
				( Frustum[ i ].y < 0.0f ) ? XMVectorGetY( p ) : XMVectorGetY( p ) + sizey,
				( Frustum[ i ].z < 0.0f ) ? XMVectorGetZ( p ) : XMVectorGetZ( p ) + sizez, 0.0f );
			if ( ( XMVectorGetX( XMPlaneDot( plane, axisvert ) ) + Frustum[ i ].w ) < 0.0f )
			{
				test = FALSE;
				break;
			}
		}
		if ( test )
			return TRUE;

		return FALSE;
	}
	inline bool isCellInFrustum( DirectX::XMFLOAT3 Center, float width,
		float Height,
		ViewFrustum const& Frustum )
	{
		if ( isSphereInFrustum( Center, width * 1.41f, Frustum ) )
			return true;
		if ( isRectangleInFrustum( Center, width, width, width, Frustum ) )
			return true;
		
		return false;
	}
}
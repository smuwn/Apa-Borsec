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
			) < -radius )
				return false;
		}
		return true;
	}
	inline bool isCellInFrustum( DirectX::XMFLOAT3 Center, float width,
		ViewFrustum const& Frustum )
	{
		if ( isSphereInFrustum( Center, width, Frustum ) )
			return true;
		float minX = Center.x - width;
		float maxX = Center.x + width;
		float minY = Center.y;
		float maxY = Center.y;
		float minZ = Center.z - width;
		float maxZ = Center.z + width;
		if ( isAABBInFrustum( minX, minY, minZ, maxX, maxY, maxZ, Frustum ) )
			return true;
		return false;
	}
}
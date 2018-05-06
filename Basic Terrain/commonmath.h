#pragma once

#include <DirectXMath.h>

namespace Math
{
	//using namespace DirectX;
	typedef DirectX::XMFLOAT2 float2;
	typedef DirectX::XMFLOAT3 float3;
	typedef DirectX::XMFLOAT4 float4;
	using DirectX::XM_PI;
	using DirectX::XM_2PI;
	using DirectX::XM_1DIV2PI;
	using DirectX::XM_1DIVPI;
	using DirectX::XM_PIDIV2;
	using DirectX::XM_PIDIV4;



	inline bool isPointInTriangle( DirectX::XMFLOAT3 const& V1,
		DirectX::XMFLOAT3 const& V2, DirectX::XMFLOAT3 const& V3,
		DirectX::XMFLOAT3 const& Point )
	{
		using namespace DirectX;
		XMVECTOR A = XMLoadFloat3( &V1 );
		XMVECTOR B = XMLoadFloat3( &V2 );
		XMVECTOR C = XMLoadFloat3( &V3 );
		XMVECTOR P = XMLoadFloat3( &Point );

		XMVECTOR v0 = C - A;
		XMVECTOR v1 = B - A;
		XMVECTOR v2 = P - A;

		float dot00 = XMVectorGetX( XMVector3Dot( v0, v0 ) );
		float dot01 = XMVectorGetX( XMVector3Dot( v0, v1 ) );
		float dot02 = XMVectorGetX( XMVector3Dot( v0, v2 ) );
		float dot11 = XMVectorGetX( XMVector3Dot( v1, v1 ) );
		float dot12 = XMVectorGetX( XMVector3Dot( v1, v2 ) );

		float invDenom = 1.0f / ( dot00 * dot11 - dot01 * dot01 );
		float u, v;
		u = ( dot11 * dot02 - dot01 * dot12 ) * invDenom;
		v = ( dot00 * dot12 - dot01 * dot02 ) * invDenom;

		return u >= 0 && v >= 0 && u + v < 1;
	}

	/// <summary>Generating gaussian random number with mean 0 and standard deviation 1.</summary>
	inline float GaussRnd()
	{
		float u1 = rand() / (float)RAND_MAX;
		float u2 = rand() / (float)RAND_MAX;
		if (u1 < 1e-6f)
			u1 = 1e-6f;
		return sqrtf(-2 * logf(u1)) * cosf(2 * XM_PI * u2);
	}

	/// <summary>Reverses the bits from an integer</summary>
	inline int reverseInt(int n)
	{
		unsigned int output = n;
		for (int i = sizeof(n) * 8 - 1; i; --i)
		{
			output <<= 1;
			n >>= 1;
			output |= n & 1;
		}
		return output;
	}

	/// <summary>Rotates the bits left</summary>
	inline int rotateLeft(unsigned int n, unsigned int d)
	{
		return (n << d) | (n >> (32 - d));
	}

	/// <summary>Rotates the bits right</summary>
	inline int rotateRight(unsigned int n, unsigned int d)
	{
		return (n >> d) | (n << (32 - d));
	}
}
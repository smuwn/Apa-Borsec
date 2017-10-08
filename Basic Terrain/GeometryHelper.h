#pragma once

#include "commonincludes.h"

namespace GeometryGenerator
{
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;
		DirectX::XMFLOAT3 Binormal;
	};
	template <class IndexType> 
	struct MeshDataInfo
	{
		std::vector<SVertex> Vertices;
		std::vector<IndexType> Indices;
	};
	typedef MeshDataInfo<WORD> SimpleIndicesMeshData;
	typedef MeshDataInfo<DWORD> DoubleIndicesMeshData;
#if defined USE_SINGLE_WORD_FOR_INDEX
	typedef SimpleIndicesMeshData MeshData;
	typedef WORD Index;
#else
	typedef DoubleIndicesMeshData MeshData;
	typedef DWORD Index;
#endif
	inline void CreateSphere( float radius, int sliceCount, int stackCount, MeshData& Output )
	{
		using namespace DirectX;
		Output.Vertices.clear( );
		Output.Indices.clear( );

		SVertex topVertex;
		topVertex.Position = DirectX::XMFLOAT3( 0.0f, radius, 0.0f );
		topVertex.Normal = DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f );
		topVertex.Texture = DirectX::XMFLOAT2( 0.0f, 0.0f );
		topVertex.Tangent = DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f );
		topVertex.Binormal = DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f );

		Output.Vertices.push_back( topVertex );

		float phiStep = DirectX::XM_PI / stackCount;
		float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

		for ( int i = 1; i < stackCount; ++i )
		{
			float phi = i * phiStep;

			for ( int j = 0; j <= sliceCount; ++j )
			{
				float theta = j * thetaStep;

				SVertex Vertex;
				Vertex.Position.x = radius * sinf( phi ) * cosf( theta );
				Vertex.Position.y = radius * cosf( phi );
				Vertex.Position.z = radius * sinf( phi ) * sinf( theta );

				XMVECTOR Normal;
				Normal = XMLoadFloat3( &Vertex.Position );
				XMStoreFloat3( &Vertex.Normal, Normal );

				Vertex.Tangent.x = -radius * sinf( phi ) * sinf( theta );
				Vertex.Tangent.y = 0;
				Vertex.Tangent.z = radius * sinf( phi ) * cosf( theta );
				XMVECTOR Tangent;
				Tangent = XMLoadFloat3( &Vertex.Tangent );
				Tangent = XMVector3Normalize( Tangent );

				XMVECTOR Binormal;
				Binormal = XMVector3Cross( Normal, Tangent );
				XMStoreFloat3( &Vertex.Binormal, Binormal );

				Vertex.Texture.x = theta / DirectX::XM_2PI;
				Vertex.Texture.y = phi / DirectX::XM_PI;

				Output.Vertices.push_back( Vertex );
			}
		}
		SVertex bottomVertex;
		bottomVertex.Position = DirectX::XMFLOAT3( 0.0f, -radius, 0.0f );
		bottomVertex.Normal = DirectX::XMFLOAT3( 0.0f, -1.0f, 0.0f );
		bottomVertex.Texture = DirectX::XMFLOAT2( 0.0f, 1.0f );
		bottomVertex.Tangent = DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f );
		bottomVertex.Binormal = DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f );
		Output.Vertices.push_back( bottomVertex );
		for ( int i = 1; i <= sliceCount; ++i )
		{
			Output.Indices.push_back( 0 );
			Output.Indices.push_back( i + 1 );
			Output.Indices.push_back( i );
		}

		UINT firstIndex = 1;
		UINT ringVertexCount = sliceCount + 1;
		for ( int i = 0; i < stackCount - 2; ++i )
		{
			for ( int j = 0; j < sliceCount; ++j )
			{
				Output.Indices.push_back( firstIndex + i * ringVertexCount + j );
				Output.Indices.push_back( firstIndex + i * ringVertexCount + j + 1 );
				Output.Indices.push_back( firstIndex + ( i + 1 ) * ringVertexCount + j );

				Output.Indices.push_back( firstIndex + ( i + 1 ) * ringVertexCount + j );
				Output.Indices.push_back( firstIndex + i * ringVertexCount + j + 1 );
				Output.Indices.push_back( firstIndex + ( i + 1 ) * ringVertexCount + j + 1 );
			}
		}

		UINT SouthPoleIndex = Output.Vertices.size( ) - 1;
		firstIndex = SouthPoleIndex - ringVertexCount;
		for ( int i = 0; i < sliceCount; ++i )
		{
			Output.Indices.push_back( SouthPoleIndex );
			Output.Indices.push_back( firstIndex + i );
			Output.Indices.push_back( firstIndex + i + 1 );
		}
	}

	inline void CreateCurbedPlane( float PlaneWidth, float PlaneResolution,
		float PlaneTop, float PlaneBottom, int TextureRepeat, MeshData& Output )
	{
		Output.Vertices.clear( );
		Output.Indices.clear( );

		float quadSize = PlaneWidth / PlaneResolution;
		float halfWidth = PlaneWidth / 2.0f;
		float constant = ( PlaneTop - PlaneBottom ) / ( halfWidth * halfWidth);
		float textureDelta = ( float ) TextureRepeat / PlaneWidth;
		float x, y, z;
		float u, v;
		Output.Vertices.resize( ( UINT ) ( PlaneResolution + 1 )  * ( UINT ) ( PlaneResolution + 1 ) );

		for ( int i = 0; i <= PlaneResolution; ++i )
		{
			z = -halfWidth + i * quadSize;
			v = textureDelta * i;
			for ( int j = 0; j <= PlaneResolution; ++j )
			{
				x = -halfWidth + j * quadSize;
				y = PlaneTop - constant * ( ( x * x ) + ( z*z ) );
				
				u = textureDelta * j;

				int index = i * ( int ) ( PlaneResolution + 1 ) + j;
				Output.Vertices[ index ].Position = DirectX::XMFLOAT3( x, y, z );
				Output.Vertices[ index ].Texture = DirectX::XMFLOAT2( u, v );
			}
		}

		int topLeftIndex;
		int topRightIndex;
		int bottomLeftIndex;
		int bottomRightIndex;
		for ( int i = 0; i < PlaneResolution; ++i )
		{
			for ( int j = 0; j < PlaneResolution; ++j )
			{
				topLeftIndex = i * ( int ) (PlaneResolution + 1) + j;
				topRightIndex = i * ( int ) (PlaneResolution + 1) + j + 1;
				bottomLeftIndex = ( i + 1 ) * ( int ) (PlaneResolution + 1) + j;
				bottomRightIndex = ( i + 1 ) * ( int ) ( PlaneResolution + 1 ) + j + 1;

				Output.Indices.push_back( topLeftIndex );
				Output.Indices.push_back( topRightIndex );
				Output.Indices.push_back( bottomLeftIndex );

				Output.Indices.push_back( bottomLeftIndex );
				Output.Indices.push_back( topRightIndex );
				Output.Indices.push_back( bottomRightIndex );
			}
		}
	}
}
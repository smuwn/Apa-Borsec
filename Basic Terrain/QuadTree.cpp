#include "QuadTree.h"



QuadTree::QuadTree( ID3D11Device * Device, ID3D11DeviceContext * Context,
	std::shared_ptr<C3DShader> Shader,
	std::shared_ptr<CTerrain> Terrain, std::shared_ptr<CLineManager> Lines ) :
	mTerrain( Terrain ),
	mLines( Lines ),
	mDevice( Device ),
	mContext( Context ),
	mShader( Shader )
{
	try
	{
		float centerX, centerZ, width;

		mVertexCount = mTerrain->mVertexCount;
		mIndexCount = mTerrain->mIndexCount;
		mFaceCount = mIndexCount / 3;

		mVertices = new SVertex[ mVertexCount ];
		mIndices = new DWORD[ mIndexCount ];

		memcpy( mVertices, &mTerrain->mVertices[ 0 ], sizeof( SVertex ) * mVertexCount );
		memcpy( mIndices, &mTerrain->mIndices[ 0 ], sizeof( DWORD )*mIndexCount );

		CalculateMeshDimensions( centerX,centerZ,width );

		mParentNode = new SNode( );
		mParentNode->mCenterX = centerX;
		mParentNode->mCenterZ = centerZ;
		mParentNode->mWidth = width;
		mParentNode->mTriangleCount = NumTrianglesInside( centerX, centerZ, width );
		BuildNode( mParentNode );

		delete[] mVertices;
		delete[] mIndices;
		mTexture = std::shared_ptr<CTexture>( mTerrain->mTexture );
		mTerrain.reset( );
	}
	CATCH;
}


QuadTree::~QuadTree( )
{
	if ( mParentNode )
	{
		ReleaseNode( mParentNode );
		delete mParentNode;
		mParentNode = 0;
	}
}

void QuadTree::BuildNode( SNode* Node )
{
	for ( int i = 0; i < 4; ++i )
		Node->mNodes[ i ] = 0;

	if ( Node->mTriangleCount == 0 )
		return;
	else if ( Node->mTriangleCount > MAX_TRIANGLES )
	{
		for ( int i = 0; i < 4; ++i )
		{
			float offsetX = ( i % 2 < 1 ? -1.0f : 1.0f ) * ( Node->mWidth / 4.0f );
			float offsetZ = ( i % 4 < 2 ? -1.0f : 1.0f ) * ( Node->mWidth / 4.0f );
			int New = NumTrianglesInside( Node->mCenterX + offsetX,
				Node->mCenterZ + offsetZ, Node->mWidth / 2.0f );
			if ( New > 0 )
			{
				Node->mNodes[ i ] = new SNode;
				Node->mNodes[ i ]->mCenterX = Node->mCenterX + offsetX;
				Node->mNodes[ i ]->mCenterZ = Node->mCenterZ + offsetZ;
				Node->mNodes[ i ]->mWidth = Node->mWidth / 2.0f;
				Node->mNodes[ i ]->mTriangleCount = New;
				BuildNode( Node->mNodes[ i ] );
			}
		}
	}
	else
	{
		SVertex * vertices = new SVertex[ Node->mTriangleCount * 3 ];

		int index = 0;
		for ( int i = 0; i < mFaceCount; ++i )
		{
			if ( isTriangleContained( i, Node->mCenterX, Node->mCenterZ, Node->mWidth ) )
			{
				vertices[ index + 0 ] = mVertices[ mIndices[ i * 3 + 0 ] ];
				vertices[ index + 1 ] = mVertices[ mIndices[ i * 3 + 1 ] ];
				vertices[ index + 2 ] = mVertices[ mIndices[ i * 3 + 2 ] ];
				index += 3;
			}
		}
		
		ShaderHelper::CreateBuffer(
			mTerrain->mDevice, &Node->mVertexBuffer,
			D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER,
			sizeof( SVertex ) * Node->mTriangleCount * 3, 0, vertices
		);

		delete vertices;
	}
	
}

void QuadTree::CalculateMeshDimensions( float& centerX, float& centerZ, float& width )
{
	centerX = 0;
	centerZ = 0;
	for ( int i = 0; i < mVertexCount; ++i )
	{
		centerX += mVertices[ i ].Position.x;
		centerZ += mVertices[ i ].Position.z;
	}
	centerX /= ( float ) mVertexCount;
	centerZ /= ( float ) mVertexCount;

	float maxWidth = fabs( mVertices[ 0 ].Position.x - centerX );
	float maxDepth = fabs( mVertices[ 0 ].Position.z - centerZ );

	for ( int i = 1; i < mVertexCount; ++i )
	{
		float width = fabs( mVertices[ i ].Position.x - centerX );
		float depth = fabs( mVertices[ i ].Position.z - centerZ );
		if ( width > maxWidth )
			maxWidth = width;
		if ( depth > maxDepth )
			maxDepth = depth;
	}
	
	width = max( maxWidth, maxDepth ) * 2.0f;
}

int QuadTree::NumTrianglesInside( float CenterX, float CenterZ, float width )
{
	int result = 0;
	for ( int i = 0; i < mFaceCount; ++i )
	{
		if ( isTriangleContained( i, CenterX, CenterZ, width ) )
		{
			result++;
		}
	}
	return result;
}

bool QuadTree::isTriangleContained( int index, float centerX, float centerZ, float width )
{
	float x1, x2, x3;
	float z1, z2, z3;
	
	float radius = width / 2.0f;

	x1 = mVertices[ mIndices[ index * 3 + 0 ] ].Position.x;
	z1 = mVertices[ mIndices[ index * 3 + 0 ] ].Position.z;

	x2 = mVertices[ mIndices[ index * 3 + 1 ] ].Position.x;
	z2 = mVertices[ mIndices[ index * 3 + 1 ] ].Position.z;

	x3 = mVertices[ mIndices[ index * 3 + 2 ] ].Position.x;
	z3 = mVertices[ mIndices[ index * 3 + 2 ] ].Position.z;

	float minimX = min( x1, min( x2, x3 ) );
	if ( minimX > centerX + radius )
		return false;

	float maximX = max( x1, max( x2, x3 ) );
	if ( maximX < centerX - radius )
		return false;

	float minimZ = min( z1, min( z2, z3 ) );
	if ( minimZ > centerZ + radius )
		return false;

	float maximZ = max( z1, max( z2, z3 ) );
	if ( maximZ < centerZ - radius )
		return false;

	return true;
}

void QuadTree::ReleaseNode( SNode* Node )
{
	for ( int i = 0; i < 4; ++i )
	{
		if ( Node->mNodes[ i ] != 0 )
		{
			ReleaseNode( Node->mNodes[ i ] );
			delete Node->mNodes[ i ];
			Node->mNodes[ i ] = 0;
		}
	}
	Node->mVertexBuffer.Reset( );
}

void QuadTree::RenderLines( )
{
	RenderNodeLines( mParentNode );
}

void QuadTree::RenderNodeLines( QuadTree::SNode* Node  )
{
	int count = 0;
	for ( int i = 0; i < 4; ++i )
	{
		if ( Node->mNodes[ i ] != 0 )
		{
			RenderNodeLines( Node->mNodes[ i ] );
			count++;
		}
	}
	if ( count > 0 )
		return;
	float minX = Node->mCenterX - Node->mWidth / 2.0f;
	float maxX = Node->mCenterX + Node->mWidth / 2.0f;
	float minZ = Node->mCenterZ - Node->mWidth / 2.0f;
	float maxZ = Node->mCenterZ + Node->mWidth / 2.0f;
	float minY = 0.0f - Node->mWidth / 2.0f;
	float maxY = 0.0f + Node->mWidth / 2.0f;
	mLines->Line( DirectX::XMFLOAT3( minX, minY, minZ ),
		DirectX::XMFLOAT3( minX,maxY,minZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, minY, minZ ),
		DirectX::XMFLOAT3( maxX, minY, minZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, minY, minZ ),
		DirectX::XMFLOAT3( minX, minY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, maxY, minZ ),
		DirectX::XMFLOAT3( maxX, maxY, minZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, maxY, minZ ),
		DirectX::XMFLOAT3( minX, maxY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( maxX, minY, minZ ),
		DirectX::XMFLOAT3( maxX, minY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( maxX, minY, minZ ),
		DirectX::XMFLOAT3( maxX, maxY, minZ ) );
	mLines->Line( DirectX::XMFLOAT3( maxX, maxY, minZ ),
		DirectX::XMFLOAT3( maxX, maxY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( maxX, maxY, maxZ ),
		DirectX::XMFLOAT3( minX, maxY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( maxX, maxY, maxZ ),
		DirectX::XMFLOAT3( maxX, minY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, maxY, maxZ ),
		DirectX::XMFLOAT3( minX, minY, maxZ ) );
	mLines->Line( DirectX::XMFLOAT3( minX, minY, maxZ ),
		DirectX::XMFLOAT3( maxX, minY, maxZ ) );
}

void QuadTree::RenderNode( SNode * Node, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
	FrustumCulling::ViewFrustum const& Frustum, int& DrawnVertices, float CamHeight )
{
	float minX = Node->mCenterX - Node->mWidth / 2.0f;
	float minY = 0.0f;
	float minZ = Node->mCenterZ - Node->mWidth / 2.0f;
	float maxX = Node->mCenterX + Node->mWidth / 2.0f;
	float maxY = 0.0f;
	float maxZ = Node->mCenterZ + Node->mWidth / 2.0f;
	//if ( !FrustumCulling::isAABBInFrustum( minX, minY, minZ, maxX, maxY, maxZ, Frustum ) )
		//return;
	if ( !FrustumCulling::isCellInFrustum(
		DirectX::XMFLOAT3( Node->mCenterX, CamHeight, Node->mCenterZ ),
		Node->mWidth / 2.0f,CamHeight, Frustum ) )
		return;
	
	int count = 0;
	for ( int i = 0; i < 4; ++i )
	{
		if ( Node->mNodes[ i ] != 0 )
		{
			RenderNode( Node->mNodes[ i ], View, Projection, Frustum, DrawnVertices,CamHeight );
		}
	}
	if ( count > 0 )
		return;

	static UINT Stride = sizeof( SVertex );
	static UINT Offset = 0;

	mContext->IASetVertexBuffers( 0, 1,
		Node->mVertexBuffer.GetAddressOf( ), &Stride, &Offset );
	mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	mShader->RenderVertices( Node->mTriangleCount * 3,
		DirectX::XMMatrixIdentity( ), View, Projection, mTexture.get( ) );
	DrawnVertices += Node->mTriangleCount * 3;

}

void QuadTree::Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
	FrustumCulling::ViewFrustum const& Frustum, int& DrawnTriangles, float CamHeight )
{
	RenderNode( mParentNode, View, Projection, Frustum, DrawnTriangles, CamHeight );
}
#pragma once

#include "Terrain.h"
#include "CLineManager.h"
#include "FrustumHelper.h"

class QuadTree sealed
{
private:
	static constexpr const unsigned int MAX_TRIANGLES = 50000;
public:
	typedef CTerrain::SVertex SVertex;
	struct SNode
	{
		float mCenterX;
		float mCenterZ;
		float mWidth;
		int mTriangleCount;
		DirectX::XMFLOAT3 * mVertices;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		SNode* mNodes[ 4 ];
	};
private:
	std::shared_ptr<CTerrain> mTerrain;
	std::shared_ptr<CLineManager> mLines;
	std::shared_ptr<C3DShader> mShader;
	std::shared_ptr<CTexture> mGrass;
	std::shared_ptr<CTexture> mSlope;
	std::shared_ptr<CTexture> mRock;

	SNode * mParentNode;

	SVertex * mVertices;
	DWORD * mIndices;

	int mVertexCount;
	int mIndexCount;
	int mFaceCount;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;

public:
	QuadTree( ID3D11Device * Device, ID3D11DeviceContext * Context,
		std::shared_ptr<C3DShader> Shader,
		std::shared_ptr<CTerrain> Terrain, std::shared_ptr<CLineManager> Lines );
	~QuadTree( );
public:
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		FrustumCulling::ViewFrustum const& Frustum, int& DrawnTriangles, float CamHeight,
		bool bWireframe = false);
	void RenderLines( );
	bool GetHeightAt( float X, float Z, float& height );
private:
	void CalculateMeshDimensions( float& CenterX, float& CenterZ, float& width );
	int NumTrianglesInside( float CenterX, float CenterZ, float width );
	bool isTriangleContained( int index, float centerX, float centerZ, float width );
	void BuildNode( SNode* Node );

	void ReleaseNode( SNode* Node );
	
	void RenderNodeLines( SNode* Node );
	void RenderNode( SNode * Node, DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection,
		FrustumCulling::ViewFrustum const& Frustum, int& DrawnTriangles, float CamHeight );

	SNode* FindNode( SNode* Start, float X, float Z );
	bool GetTriangleHeight( float X, float Z, float& height,
		DirectX::XMFLOAT3& V1, DirectX::XMFLOAT3& V2, DirectX::XMFLOAT3& V3 );
};


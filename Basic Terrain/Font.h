#pragma once

#include "commonincludes.h"
#include "Texture.h"


class CFont
{
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Texture;
	};
	struct SLetter
	{
		short ID;
		float u; // (0-1) - Start U
		float v; // (0-1) - Start V
		float width; // (0-1) - End U = u+width
		float height; // (0-1) - End V = v+height
		float xOffset; // (0-1) - in texture space
		float yOffset; // (0-1) - in texture space
		float xAdvance; // Advance X pixels when rendering
		float swidth;
		float sheight;
	};
	struct SKerning
	{
		int first; // First character
		int second; // Second character
		int amount; // Pixels
	};
private:
	std::unique_ptr<CTexture> mTexture;
	std::unordered_map<size_t,SLetter*> mLetters;
	std::vector<SKerning> mKernings;
	std::array<int, 4> mPadding;
	std::wstring mName;
	int mSize;
	int mLineHeight;
	int mTextureWidth;
	int mTextureHeight;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CFont( ID3D11Device * Device, ID3D11DeviceContext * Context, LPWSTR lpPath  );
	~CFont( );
public:
	CTexture * GetTexture( ) { return mTexture.get( ); };
	void Build( void* Vertices, void* Indices,
		UINT& VertexCount, UINT& IndexCount,
		LPCSTR Message, float x, float y );
private:
	void ReadFile( LPWSTR );
};


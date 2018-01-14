#pragma once



#include "commonincludes.h"



class Utilities
{
	/*****************Random texture*****************/
	static constexpr const unsigned int RandomTextureSize = 1024;
	static Microsoft::WRL::ComPtr<ID3D11Texture1D> mRandomTexture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mRandomTextureSRV;
	/*****************Random texture*****************/
public:
	static void Create( ID3D11Device * );
	static DirectX::XMFLOAT4 GetRandomDirection( );
public: // Getters
	static ID3D11Texture1D* GetRandomTexture( ) { return mRandomTexture.Get( ); };
	static ID3D11ShaderResourceView* GetRandomTextureSRV( ) { return mRandomTextureSRV.Get( ); };
};


#pragma once

#include "commonincludes.h"
#include "LineShader.h"

class CLineManager
{
	static constexpr const unsigned int MAX_LINES = 1024*1024;
public:
	struct SVertex
	{
		DirectX::XMFLOAT3 Position;
	};
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertBuffer;

	std::shared_ptr<LineShader> mShader;

	void* mAddress;
	int mVertexToAdd = 0;

	ID3D11Device * mDevice;
	ID3D11DeviceContext * mContext;
public:
	CLineManager( ID3D11Device* Device, ID3D11DeviceContext* Context,
		std::shared_ptr<LineShader> Shader );
	~CLineManager( );
public:
	void Begin( );
	void End( );
	void Line( DirectX::XMFLOAT3 const& From, DirectX::XMFLOAT3 const& To );
	void Render( DirectX::FXMMATRIX& View, DirectX::FXMMATRIX& Projection );
};


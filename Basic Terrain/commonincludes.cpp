#include "commonincludes.h"

namespace DX
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> DefaultRS;
	void InitStates( ID3D11Device * Device )
	{
		ZeroMemoryAndDeclare( D3D11_RASTERIZER_DESC, rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &NoCulling ) );
		ZeroVariable( rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &DefaultRS ) );
	}
}
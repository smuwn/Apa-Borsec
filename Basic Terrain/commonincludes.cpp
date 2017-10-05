#include "commonincludes.h"

namespace DX
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> FrontCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> DefaultRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> Wireframe;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DS2D;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSLessEqual;
	void InitStates( ID3D11Device * Device )
	{
		ZeroMemoryAndDeclare( D3D11_RASTERIZER_DESC, rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &FrontCulling ) );
		ZeroVariable( rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &NoCulling ) );
		ZeroVariable( rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &DefaultRS ) );
		ZeroVariable( rastDesc );
		rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		ThrowIfFailed( Device->CreateRasterizerState( &rastDesc, &Wireframe ) );
		ZeroMemoryAndDeclare( D3D11_DEPTH_STENCIL_DESC, dsDesc );
		dsDesc.DepthEnable = FALSE;
		dsDesc.StencilEnable = FALSE;
		ThrowIfFailed( Device->CreateDepthStencilState( &dsDesc, &DS2D ) );
		ZeroVariable( dsDesc );
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.StencilEnable = FALSE;
		ThrowIfFailed( Device->CreateDepthStencilState( &dsDesc, &DSLessEqual ) );
	}
}
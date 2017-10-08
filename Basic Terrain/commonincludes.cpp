#include "commonincludes.h"

namespace DX
{
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> FrontCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> NoCulling;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> DefaultRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> Wireframe;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DS2D;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSLessEqual;
	Microsoft::WRL::ComPtr<ID3D11BlendState> AdditiveBlend;
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
		ZeroMemoryAndDeclare( D3D11_BLEND_DESC, blendDesc );
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[ 0 ].BlendEnable = TRUE;
		blendDesc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		ThrowIfFailed( Device->CreateBlendState( &blendDesc, &AdditiveBlend ) );
	}
}
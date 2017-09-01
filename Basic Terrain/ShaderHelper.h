#pragma once

#include "commonincludes.h"


namespace ShaderHelper
{

	inline void CreateShaderFromFile( LPWSTR filePath, LPSTR profile,
		ID3D11Device * device, ID3DBlob ** ShaderBlob, ID3D11DeviceChild ** Shader )
	{

		DX::ThrowIfFailed( D3DReadFileToBlob( filePath, ShaderBlob ) );

		switch ( profile[ 0 ] )
		{
		case 'v': // Vertex Shader
		DX::ThrowIfFailed(
			device->CreateVertexShader( ( *ShaderBlob )->GetBufferPointer( ), ( *ShaderBlob )->GetBufferSize( ), nullptr,
				reinterpret_cast< ID3D11VertexShader** >( Shader ) )
			);
		break;
		case 'h': // Hull shader
		DX::ThrowIfFailed(
			device->CreateHullShader( ( *ShaderBlob )->GetBufferPointer( ), ( *ShaderBlob )->GetBufferSize( ), nullptr,
				reinterpret_cast< ID3D11HullShader** > ( Shader ) )
			);
		break;
		case 'd': // Domain Shader
		DX::ThrowIfFailed(
			device->CreateDomainShader( ( *ShaderBlob )->GetBufferPointer( ), ( *ShaderBlob )->GetBufferSize( ), nullptr,
				reinterpret_cast< ID3D11DomainShader** > ( Shader ) )
			);
		break;
		case 'g': // Geometry Shader
		DX::ThrowIfFailed(
			device->CreateGeometryShader( ( *ShaderBlob )->GetBufferPointer( ), ( *ShaderBlob )->GetBufferSize( ), nullptr,
				reinterpret_cast< ID3D11GeometryShader** > ( Shader ) )
			);
		break;
		case 'p': // Pixel Shader
		DX::ThrowIfFailed(
			device->CreatePixelShader( ( *ShaderBlob )->GetBufferPointer( ), ( *ShaderBlob )->GetBufferSize( ), nullptr,
				reinterpret_cast< ID3D11PixelShader** > ( Shader ) )
			);
		break;
		}

	}

}
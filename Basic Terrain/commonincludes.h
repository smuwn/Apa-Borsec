#pragma once


#define UNICODE

#include <windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <d3d10.h>
#include <D3DX10.h>
#include <dxgi.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include <random>
#include <string>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3d10.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment (lib, "dxguid.lib")


#define ZeroMemoryAndDeclare(type, name) type name;\
ZeroMemory(sizeof(type),&name);

namespace DX
{
	inline void ThrowIfFailed( HRESULT hr )
	{
		if ( FAILED( hr ) )
			throw "DirectX Error";
	};
	inline void OutputVDebugString( const wchar_t * format, ... )
	{
		static wchar_t Sequence[ 1024 ];
		va_list args;
		va_start( args, format );
		_vsnwprintf_s( Sequence, sizeof( Sequence ), format, args );
		va_end( args );
		OutputDebugStringW( Sequence );
	}
	inline void SafeRelease( IUnknown *& object )
	{
		OutputVDebugString( L"Please don't use SafeRelease(); Use smart pointers instead" );
		if ( object )
		{
			object->Release( );
			object = nullptr;
		}
	}
}

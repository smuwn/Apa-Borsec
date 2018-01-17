#include "Rain.hlsli"

struct GSIn
{
	float4 Position : SV_POSITION;
	float Type : TYPE;
};

struct GSOut
{
	float4 pos : SV_POSITION;
};

[maxvertexcount( 2 )]
void main(
	point GSIn input[ 1 ],
	inout LineStream< GSOut > result
	)
{
	if ( input[ 0 ].Type != EMITTER )
	{
		float3 p0 = input[ 0 ].Position.xyz;
		float3 p1 = input[ 0 ].Position.xyz + 0.1f * gAccelW;

		GSOut v0;
		v0.pos = mul( float4( p0, 1.0f ), gViewProj );
		result.Append( v0 );

		GSOut v1;
		v1.pos = mul( float4( p1, 1.0f ), gViewProj );
		result.Append( v1 );
	}
}
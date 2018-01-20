#include "Fireworks.hlsli"

struct GSIn
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float3 ParticleDir : DIRECTION;
	float2 Size : SIZE;
	float Type : TYPE;
};

struct GSOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

[maxvertexcount( 4 )]
void main(
	point GSIn input[ 1 ],
	inout TriangleStream< GSOut > result
	)
{
	float2 texCoords[ 4 ] =
	{
		float2( 0.0f, 1.0f ),
		float2( 1.0f, 1.0f ),
		float2( 0.0f, 0.0f ),
		float2( 1.0f, 0.0f )
	};
	if ( input[ 0 ].Type != EMITTER )
	{

		float3 middle = input[ 0 ].Position.xyz;
		float3 forward = normalize( gEyePosW - input[ 0 ].Position.xyz );
		float3 right = normalize( cross( float3( 0, 1, 0 ), forward ) );
		float3 up = normalize( cross( forward, right ) );

		float halfWidth = input[ 0 ].Size.x / 2.0f;
		float halfHeight = input[ 0 ].Size.y / 2.0f;

		float4 v[ 4 ];
		v[ 0 ] = float4( middle + halfWidth*right - halfHeight*up, 1.0f );
		v[ 1 ] = float4( middle + halfWidth*right + halfHeight*up, 1.0f );
		v[ 2 ] = float4( middle - halfWidth*right - halfHeight*up, 1.0f );
		v[ 3 ] = float4( middle - halfWidth*right + halfHeight*up, 1.0f );

		for ( int i = 0; i < 4; ++i )
		{
			GSOut output;
			output.pos = mul( v[ i ], gViewProj );
			output.color = input[ 0 ].Color;
			output.texcoord = texCoords[ i ];

			result.Append( output );
		}

	}
}
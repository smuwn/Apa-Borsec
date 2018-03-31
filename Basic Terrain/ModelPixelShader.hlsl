Texture2D ObjTexture : register( t0 );
SamplerState ObjWrapSampler : register( s0 );

Texture2D ObjShadowMap : register( t10 );
SamplerComparisonState ComparisonSampler : register( s1 );

cbuffer cbPerObject : register( b0 )
{
	float4 gObjectColor;
}

cbuffer cbLights : register( b1 )
{
	float4 gLightColor;
	float3 gLightPosition;
	float pad;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float4 LightPositionH : POSITION1;
	float3 PositionW : POSITION;
	float3 Normal : NORMAL;
	float2 Texture : TEXCOORD;
};

float CalcShadowMap( float3 projectedTexCoord )
{
	float lightDepth = projectedTexCoord.z;

	float dx = 1.0f / 2048.f;

	float2 offsets[ 9 ] =
	{
		float2( -dx,  -dx ), float2( 0.0f,  -dx ), float2( dx,  -dx ),
		float2( -dx, 0.0f ), float2( 0.0f, 0.0f ), float2( dx, 0.0f ),
		float2( -dx,  +dx ), float2( 0.0f,  +dx ), float2( dx,  +dx )
	};

	float percentLit = 0.0f;
	for ( int i = 0; i < 9; ++i )
	{
		float2 newTexCoord = projectedTexCoord.xy + offsets[ i ];
		percentLit += ObjShadowMap.SampleCmpLevelZero( ComparisonSampler,
			newTexCoord,
			lightDepth ).r;
	}
	percentLit /= 9.0f;
	return percentLit;
}

float4 main(PSIn input) : SV_TARGET
{
	float4 diffse = gObjectColor;
	float4 color = float4( 0.2f, 0.2f, 0.2f, 0.2f );

	float3 projectedTexCoord;
	projectedTexCoord.x = input.LightPositionH.x / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.y = -input.LightPositionH.y / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.z = input.LightPositionH.z / input.LightPositionH.w;

	if ( projectedTexCoord.x == saturate( projectedTexCoord.x ) && projectedTexCoord.y == saturate( projectedTexCoord.y ) )
	{
		float lightDepth = projectedTexCoord.z;

		float dx = 1.0f / 2048.f;

		float2 offsets[ 9 ] =
		{
			float2( -dx,  -dx ), float2( 0.0f,  -dx ), float2( dx,  -dx ),
			float2( -dx, 0.0f ), float2( 0.0f, 0.0f ), float2( dx, 0.0f ),
			float2( -dx,  +dx ), float2( 0.0f,  +dx ), float2( dx,  +dx )
		};

		float percentLit = 0.0f;
		[unroll]
		for ( int i = 0; i < 9; ++i )
		{
			float2 newTexCoord = projectedTexCoord.xy + offsets[ i ];
			float texDepth = ObjShadowMap.Sample( ObjWrapSampler, newTexCoord ).r;
			percentLit += ObjShadowMap.SampleCmpLevelZero( ComparisonSampler,
				newTexCoord,
				lightDepth ).r;
		}
		percentLit /= 9.0f;
		float3 toLight = normalize( gLightPosition - input.PositionW );
		float howMuchLight = dot( toLight, input.Normal );
		if ( howMuchLight > 0 )
		{
			color += ( howMuchLight * gLightColor ) * percentLit;
		}
	}

	return saturate( color * gObjectColor );
}
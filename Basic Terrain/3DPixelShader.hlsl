
Texture2D ObjGrass : register( t0 );
Texture2D ObjTexture2 : register( t1 );
Texture2D ObjAlphamap : register( t2 );
Texture2D ObjSlope : register( t3 );
Texture2D ObjRock : register( t4 );
SamplerState WrapSampler : register( s0 );

Texture2D ObjShadowMap : register( t10 );
SamplerState ClampSampler : register( s1 );
SamplerComparisonState ComparisonSampler : register( s2 );

#define MIN_SLOPE 0.2f
#define MAX_SLOPE 0.7f

cbuffer cbLight : register( b0 )
{
    float3 LightPosition;
    float pad;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer cbTextures : register( b1 )
{
    bool HasAlpha;
}

struct PSIn
{
	float4 PositionH : SV_POSITION;
	float4 PositionW : POSITION;
	float4 PositionHC : POSITION1; // C - Copy
	float4 LightPositionH : POSITION2;
	float4 Color : COLOR;
	float4 TexCoord : TEXCOORD;
	float3 NormalW : NORMAL;
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
		percentLit += ObjShadowMap.SampleCmpLevelZero( ComparisonSampler,
			projectedTexCoord.xy + offsets[ i ], lightDepth ).r;
	}
	percentLit /= 9.0f;
	return percentLit;
}

float4 main( PSIn input ) : SV_TARGET
{
    input.NormalW = normalize( input.NormalW );
    //float4 Color = float4( 0.0f, 0.36f, 0.036f, 1.0f );

	float3 projectedTexCoord;
	projectedTexCoord.x = input.LightPositionH.x / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.y = -input.LightPositionH.y / input.LightPositionH.w / 2.0f + 0.5f;
	projectedTexCoord.z = input.LightPositionH.z / input.LightPositionH.w;

    float4 Multiplier = Ambient;
	
	if ( projectedTexCoord.x == saturate( projectedTexCoord.x ) && projectedTexCoord.y == saturate( projectedTexCoord.y ) )
	{
		float percentLit = CalcShadowMap( projectedTexCoord );
		float3 toLight = normalize( LightPosition - input.PositionW.xyz );
		float howMuchLight = dot( input.NormalW, toLight );
		if ( howMuchLight > 0.0f )
		{
			Multiplier += ( Diffuse * howMuchLight ) * percentLit;
		}
	}

    float4 BlendColor;
    float4 Color;

    if ( HasAlpha )
    {
        float4 t1 = ObjGrass.Sample( WrapSampler, input.TexCoord.xy );
        float4 t2 = ObjTexture2.Sample( WrapSampler, input.TexCoord.xy );
        float4 alpha = ObjAlphamap.Sample( WrapSampler, input.TexCoord.xy );
        BlendColor = lerp( t1, t2, alpha );
    }
	else
    {
        float2 texCoord;
        float depth = input.PositionHC.z / input.PositionHC.w;
        if ( depth < 0.99f )
        {
            texCoord = input.TexCoord.zw;
        }
        else
        {
            texCoord = input.TexCoord.xy;
        } /// Detail control

        float4 GrassColor = ObjGrass.Sample( WrapSampler, texCoord );
        float4 SlopeColor = ObjSlope.Sample( WrapSampler, texCoord );
        float4 RockColor = ObjRock.Sample( WrapSampler, texCoord );

        float InvSlope = 1.0f - input.NormalW.y;

        if ( InvSlope < MIN_SLOPE )
        {
            float blend = InvSlope / MIN_SLOPE;
            BlendColor = lerp( GrassColor, SlopeColor, blend );
        }
        else if ( InvSlope >= MIN_SLOPE && InvSlope < MAX_SLOPE )
        {
            float blend = ( InvSlope - MIN_SLOPE ) / ( MAX_SLOPE - MIN_SLOPE );
            BlendColor = lerp( SlopeColor, RockColor, blend );
        }
		else // InvSlope >= MAX_SLOPE
        {
            BlendColor = RockColor;
        }
    }
    Color = BlendColor * Multiplier;


    return saturate(Color * input.Color * 2.0f);
}
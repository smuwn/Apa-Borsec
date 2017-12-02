
Texture2D ObjGrass : register( t0 );
Texture2D ObjTexture2 : register( t1 );
Texture2D ObjAlphamap : register( t2 );
Texture2D ObjSlope : register( t3 );
Texture2D ObjRock : register( t4 );
SamplerState WrapSampler : register( s0 );

#define MIN_SLOPE 0.2f
#define MAX_SLOPE 0.7f

cbuffer cbLight : register( b0 )
{
    float3 Direction;
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
    float4 PositionHC : POSITION1;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float3 NormalW : NORMAL;
    float Clip : SV_CLIPDISTANCE0;
};

float4 main( PSIn input ) : SV_TARGET
{
    input.NormalW = normalize( input.NormalW );
    //float4 Color = float4( 0.0f, 0.36f, 0.036f, 1.0f );

    float4 Multiplier = Ambient;

    float3 LightDir = -Direction;

    float howMuchLight = dot( input.NormalW, LightDir );
    if ( howMuchLight > 0.0f )
    {
        Multiplier += Diffuse * howMuchLight;
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
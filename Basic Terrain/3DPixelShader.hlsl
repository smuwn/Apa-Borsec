
Texture2D ObjGrass : register( t0 );
Texture2D ObjTexture2 : register( t1 );
Texture2D ObjAlphamap : register( t2 );
Texture2D ObjSlope : register( t3 );
Texture2D ObjRock : register( t4 );
Texture2D ObjBumpMap : register( t5 );
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
    float3 TangentW : TANGENT;
    float3 BinormalW : BINORMAL;
};

float4 ComputeColor( float4 Ambient, float3 NormalW, float4 PositionW, float4 Diffuse, float3 Direction )
{
    float3 LightDir = -Direction;

    float howMuchLight = dot( NormalW, LightDir );
    if ( howMuchLight > 0.0f )
    {
        Ambient += Diffuse * howMuchLight;
    }
    return Ambient;
}

float3 NormalmapSpaceToWorldSpace( float3 NormalW, float3 NormalmapSample, float3 TangentW, float3 BinormalW )
{
    float3 normalT = 2.0f * NormalmapSample - 1.0f;
    float3 N = NormalW;
    float3 T = normalize( TangentW - dot( TangentW, N ) * N );
    float3 B = BinormalW;
    float3x3 TBN = float3x3( T, B, N );

    float3 bumpedNormalW = mul( normalT, TBN );

    return bumpedNormalW;
}


float4 main( PSIn input ) : SV_TARGET
{
    input.NormalW = normalize( input.NormalW );
    //float4 Color = float4( 0.0f, 0.36f, 0.036f, 1.0f );

    float4 Multiplier = Ambient;

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

        float InvSlope = 1.0f - input.NormalW.y;
        if ( depth < 0.98f )
        {
            texCoord = input.TexCoord.zw;

            float4 GrassColor = ObjGrass.Sample( WrapSampler, texCoord );
            float4 SlopeColor = ObjSlope.Sample( WrapSampler, texCoord );
            float4 RockColor = ObjRock.Sample( WrapSampler, texCoord );

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
            float3 BumpmapSample = ObjBumpMap.Sample( WrapSampler, texCoord ).rgb;
            float3 Normal = NormalmapSpaceToWorldSpace( input.NormalW, BumpmapSample, input.TangentW, input.BinormalW );
            Multiplier = ComputeColor( Multiplier, Normal, input.PositionW, Diffuse, Direction );
        }
        else
        {
            texCoord = input.TexCoord.xy;

            float4 GrassColor = ObjGrass.Sample( WrapSampler, texCoord );
            float4 SlopeColor = ObjSlope.Sample( WrapSampler, texCoord );
            float4 RockColor = ObjRock.Sample( WrapSampler, texCoord );

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
			
            Multiplier = ComputeColor( Multiplier, input.NormalW, input.PositionW, Diffuse, Direction );

        } /// Detail control


    }
    Color = BlendColor * Multiplier;


    return saturate(Color * input.Color * 2.0f);
}
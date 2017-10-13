
Texture2D ObjTexture : register( t0 );
Texture2D ObjTexture2 : register( t1 );
Texture2D ObjAlphamap : register( t2 );
SamplerState WrapSampler : register( s0 );

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
        float4 t1 = ObjTexture.Sample( WrapSampler, input.TexCoord.xy );
        float4 t2 = ObjTexture2.Sample( WrapSampler, input.TexCoord.xy );
        float4 alpha = ObjAlphamap.Sample( WrapSampler, input.TexCoord.xy );
        BlendColor = lerp( t1, t2, alpha );
    }
	else
    {
        float depth = input.PositionHC.z / input.PositionHC.w;
        if ( depth < 0.95f )
        {
            BlendColor = ObjTexture.Sample( WrapSampler, input.TexCoord.zw );
        }
        else
        {
            BlendColor = ObjTexture.Sample( WrapSampler, input.TexCoord.xy );
        }
    }
    Color = BlendColor * Multiplier;

    return saturate(Color * input.Color * 2.0f);
}
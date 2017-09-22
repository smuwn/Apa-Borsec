
Texture2D ObjTexture : register( t0 );
SamplerState WrapSampler : register( s0 );

cbuffer cbLight : register( b0 )
{
    float3 Direction;
    float pad;
    float4 Diffuse;
    float4 Ambient;
};

struct PSIn
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float4 Color : COLOR;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    input.NormalW = normalize( input.NormalW );
    //float4 Color = float4( 0.0f, 0.36f, 0.036f, 1.0f );
    float4 Color = ObjTexture.Sample(WrapSampler, input.TexCoord);

    float4 Multiplier = Ambient;

    float3 LightDir = -Direction;

    float howMuchLight = dot( input.NormalW, LightDir );
    if ( howMuchLight > 0.0f )
    {
        Multiplier += Diffuse * howMuchLight;
    }

    Color = Color * Multiplier;

    return saturate(Color * input.Color * 2.0f);
}
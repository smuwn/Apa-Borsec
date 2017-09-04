
Texture2D Texture : register( t0 );
SamplerState WrapSampler : register( s0 );

struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    return float4( 1.0f, 1.0f, 0.0f, 1.0f );
}
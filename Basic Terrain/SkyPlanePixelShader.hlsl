
Texture2D ObjClouds : register( t0 );
Texture2D ObjPerturb : register( t1 );
SamplerState WrapSampler : register( s0 );

cbuffer cbPerFrame : register( b0 )
{
    float2 translation;
    float scale;
    float brightness;
};

struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    input.TexCoord += translation;

    float4 perturbValue = ObjPerturb.Sample( WrapSampler, input.TexCoord );

    perturbValue = perturbValue * scale;
    perturbValue.xy = perturbValue.xy + input.TexCoord + translation;

    float4 cloudColor = ObjClouds.Sample( WrapSampler, perturbValue.xy );

    cloudColor *= brightness;

    return cloudColor;
}
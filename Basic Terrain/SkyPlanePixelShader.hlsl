
Texture2D ObjTexture0 : register( t0 );
Texture2D ObjTexture1 : register( t1 );
SamplerState WrapSampler : register( s0 );

cbuffer cbPerFrame : register( b0 )
{
    float2 FirstTextureOffset;
    float2 SecondTextureOffset;
    float brightness;
    float3 pad;
};

struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    float2 firstInput = input.TexCoord + FirstTextureOffset;
    float4 FirstTexture = ObjTexture0.Sample( WrapSampler, firstInput );
    float2 secondInput = input.TexCoord + SecondTextureOffset;
    float4 SecondTexture = ObjTexture1.Sample( WrapSampler, secondInput );

    float4 finalColor = lerp( FirstTexture, SecondTexture, 0.5f );

    return finalColor;
}
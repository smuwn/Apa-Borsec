
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    VSOut output;
    output.Position = mul( pos, WVP );
    output.TexCoord = tex;
    return output;
}

cbuffer cbPerObject : register( b0 )
{
    float4x4 gWVP;
};

struct VSOut
{
    float4 SVPosition : SV_POSITION;
    float4 Position : POSITION;
};

VSOut main( float4 pos : POSITION )
{
    VSOut output;
    output.SVPosition = mul( pos, gWVP ).xyww;
    output.Position = pos;
    return output;
}

cbuffer cbPerObject
{
    float4x4 WVP;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float4 DepthPosition : POSITION;
};

VSOut main( float4 pos : POSITION )
{
    VSOut output = ( VSOut ) 0;
    output.Position = mul( pos, WVP );
    output.DepthPosition = output.Position;
    return output;
}
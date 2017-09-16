
cbuffer cbPerObject : register(b0)
{
    float4x4 WVP;
};

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(pos, WVP);
}
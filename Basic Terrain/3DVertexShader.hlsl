
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
};


struct VSOut
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float4 Color : COLOR;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
};


VSOut main( float3 pos : POSITION, float2 tex : TEXCOORD, float3 nor : NORMAL, float4 Color : COLOR )
{
    VSOut output = ( VSOut ) 0;
    output.PositionH = mul( float4( pos, 1.0f ), WVP );
    output.PositionW = mul( float4( pos, 1.0f ), World );
    output.NormalW = mul( float4( nor, 1.0f ), World ).xyz;
	output.Color = Color;
    output.TexCoord = tex;
	return output;
}
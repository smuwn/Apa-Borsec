
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
};


struct VSOut
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float4 PositionHC : POSITION1;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float3 BinormalW : BINORMAL;
};


VSOut main( float3 pos : POSITION, float4 tex : TEXCOORD, float3 nor : NORMAL, float3 tan : TANGENT, float3 bin : BINORMAL, float4 Color : COLOR )
{
    VSOut output = ( VSOut ) 0;
    output.PositionH = mul( float4( pos, 1.0f ), WVP );
    output.PositionW = mul( float4( pos, 1.0f ), World );
    output.PositionHC = output.PositionH;
    output.NormalW = mul( float4( nor, 1.0f ), World ).xyz;
    output.TangentW = mul( float4( tan, 1.0f ), World ).xyz;
    output.BinormalW = mul( float4( bin, 1.0f ), World ).xyz;
	output.Color = Color;
    output.TexCoord = tex;
	return output;
}

cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
};

cbuffer cbClippingPlane : register( b1 )
{
    float4 ClippingPlane;
};

struct VSOut
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float4 PositionHC : POSITION1;
    float4 Color : COLOR;
    float4 TexCoord : TEXCOORD;
    float3 NormalW : NORMAL;

    float Clip : SV_CLIPDISTANCE0;
};


VSOut main( float3 pos : POSITION, float4 tex : TEXCOORD, float3 nor : NORMAL, float4 Color : COLOR )
{
    VSOut output = ( VSOut ) 0;

    output.PositionH = mul( float4( pos, 1.0f ), WVP );
    output.PositionW = mul( float4( pos, 1.0f ), World );
    output.PositionHC = output.PositionH;
    output.NormalW = mul( float4( nor, 1.0f ), World ).xyz;
	output.Color = Color;
    output.TexCoord = tex;

    output.Clip = dot( output.PositionW, ClippingPlane );

	return output;
}
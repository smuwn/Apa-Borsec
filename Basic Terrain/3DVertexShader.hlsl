
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
    float4x4 World;
};

cbuffer cbPerLight : register( b1 )
{
	float4x4 LightViewProj;
}

cbuffer cbClippingPlane : register( b2 )
{
    float4 ClippingPlane;
};

struct VSOut
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float4 PositionHC : POSITION1; // C - Copy
	float4 LightPositionH : POSITION2;
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
	
	output.LightPositionH = mul( output.PositionW, LightViewProj );
	
    output.NormalW = mul( float4( nor, 0.0f ), World ).xyz;
	output.Color = Color;
    output.TexCoord = tex;

    output.Clip = dot( output.PositionW, ClippingPlane );

	return output;
}
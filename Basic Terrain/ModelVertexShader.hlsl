
struct VSIn
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texture : TEXCOORD;
};

struct VSOut
{
	float4 PositionH : SV_POSITION;
	float4 LightPositionH : POSITION1;
	float3 PositionW : POSITION;
	float3 Normal : NORMAL;
	float2 Texture : TEXCOORD;
};

cbuffer cbPerObject : register ( b0 )
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

cbuffer cbLight : register( b1 )
{
	float4x4 gLightView;
	float4x4 gLightProjection;
}

VSOut main( VSIn input )
{
	VSOut output;
	float4x4 ViewProjection = mul( View, Projection );
	float4x4 WVP = mul( World, ViewProjection );
	output.PositionH = mul( float4( input.Position, 1.0f ), WVP );
	output.PositionW = mul( float4( input.Position, 1.0f ), World ).xyz;

	float4x4 LWVP = mul( World, gLightView );
	LWVP = mul( LWVP, gLightProjection );
	output.LightPositionH = mul( float4( input.Position, 1.0f ), LWVP );

	output.Normal = mul( float4( input.Normal, 0.0f ), World ).xyz;
	output.Normal = normalize( output.Normal );
	output.Texture = input.Texture;
	return output;
}
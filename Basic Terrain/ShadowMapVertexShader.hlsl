
struct VSIn
{
	float3 pos : POSITION;
	float2 texCoord : TEXCOORD;
};

struct VSOut
{
	float4 Position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


cbuffer cbPerObject : register( b0 )
{
	float4x4 WVP;
};

VSOut main( VSIn input )
{
	VSOut output = ( VSOut ) 0;

	output.Position = mul( float4( input.pos, 1.0f ), WVP );
	output.texCoord = input.texCoord;

	return output;
}
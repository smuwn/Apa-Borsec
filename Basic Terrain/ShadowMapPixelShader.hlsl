
Texture2D ObjTexture : register( t0 );
SamplerState ObjSampler : register( s0 );

struct PSIn
{
	float4 Position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};


cbuffer cbPerMaterial : register( b0 )
{
	bool bHasTexture;
};

float4 main( PSIn input ) : SV_TARGET
{
	float4 color = ObjTexture.Sample( ObjSampler, input.texCoord );
	//clip( color.a - 0.15f );
	if ( color.a - 0.15f < 0.0f )
		return float4( 1.0f, 1.0f, 1.0f, 1.0f );

	return float4( 1.0f, 0.0f, 0.0f, 1.0f );
}
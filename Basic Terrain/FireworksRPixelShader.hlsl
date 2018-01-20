
Texture2D ObjTexture : register( t0 );
SamplerState WrapSampler : register( s0 );


struct PSIn
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

float4 main( PSIn input) : SV_TARGET
{
	float4 color = ObjTexture.Sample( WrapSampler, input.texcoord ) * input.color;

	clip( color.r - 0.25f );

	return color;
}
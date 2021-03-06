

Texture2D ObjTexture : register( t0 );
SamplerState ObjWrapSampler : register( s0 );


struct PSIn
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};


float4 main(PSIn input) : SV_TARGET
{
	float4 Color = ( ObjTexture.Sample( ObjWrapSampler,input.texcoord ) * input.color );
	clip( Color.a - 0.25f );
	return Color;
}
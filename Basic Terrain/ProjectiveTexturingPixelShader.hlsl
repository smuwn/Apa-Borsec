
Texture2D ObjTexture : register( t0 );
SamplerState ObjWrapSampler : register( s0 );


struct PSIn
{
	float4 Position : SV_POSITION;
	float4 ViewPosition : POSITION0;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};


float4 main( PSIn input ) : SV_TARGET
{

	float u = input.ViewPosition.x / input.ViewPosition.w / 2.0f + 0.5f;
	float v = -input.ViewPosition.y / input.ViewPosition.w / 2.0f + 0.5f;

	if ( saturate( u ) == u && saturate( v ) == v )
	{
		float4 color = ObjTexture.Sample( ObjWrapSampler, float2( u, v ) );
		return color;
	}

	return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}
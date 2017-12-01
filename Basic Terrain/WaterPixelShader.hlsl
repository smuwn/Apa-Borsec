
struct PSIn
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
	return float4(0.0f, 0.0f, 1.0f, 1.0f);
}
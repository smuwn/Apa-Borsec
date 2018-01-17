#include "Rain.hlsli"


struct VSIn
{
	float3 ParticlePos : POSITION;
	float3 ParticleDir : DIRECTION;

	float2 Size : SIZE;

	float Age : AGE;
	unsigned int Type : TYPE;
};

struct VSOut
{
	float4 ParticlePos : SV_POSITION;
	float Type : TYPE;
};

VSOut main( VSIn input )
{
	
	VSOut output;
	float t = input.Age;
	output.ParticlePos.xyz = 0.5f * t * t * gAccelW + t * input.ParticleDir + input.ParticlePos;
	output.ParticlePos.w = 1.0f;

	output.Type = input.Type;

	return output;
}
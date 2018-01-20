#include "Fireworks.hlsli"


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
	float4 Color : COLOR;
	float3 ParticleDir : DIRECTION;
	float2 Size : SIZE;
	float Type : TYPE;
};

VSOut main( VSIn input )
{
	VSOut output;
	float t = input.Age;
	output.ParticlePos.xyz = input.ParticlePos.xyz;
	output.ParticlePos.w = 1.0f;

	output.ParticleDir = input.ParticleDir;
	output.Color = float4( saturate( input.ParticleDir ), 1.0f );

	output.Type = input.Type;
	output.Size = input.Size;

	return output;
}
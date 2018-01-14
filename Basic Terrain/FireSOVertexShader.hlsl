
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
	float3 ParticleDir : DIRECTION;

	float2 Size : SIZE;

	float Age : AGE;
	unsigned int Type : TYPE;
};


VSOut main( VSIn input )
{
	VSOut output;
	output.ParticlePos.xyz = input.ParticlePos.xyz;
	output.ParticlePos.w = 1.0f;
	output.ParticleDir = input.ParticleDir;

	output.Size = input.Size;

	output.Age = input.Age;
	output.Type = input.Type;

	return output;
}
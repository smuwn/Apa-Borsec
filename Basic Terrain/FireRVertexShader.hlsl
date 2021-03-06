

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
	float2 Size : SIZE;
	float Type : TYPE;
};

VSOut main( VSIn input )
{
	float3 gAccelW = float3( 0.0f, 10.0f, 0.0f );
	VSOut output;
	float t = input.Age;
	output.ParticlePos.xyz = 0.5f * t * t * gAccelW + t * input.ParticleDir + input.ParticlePos;
	output.ParticlePos.w = 1.0f;
	
	float opacity = 1.0f - smoothstep( 0.0f, 1.0f, t / 1.0f );
	output.Color = float4( 1.0f, 1.0f, 1.0f, opacity );

	output.Size = input.Size;	

	output.Type = input.Type;	

	return output;
}
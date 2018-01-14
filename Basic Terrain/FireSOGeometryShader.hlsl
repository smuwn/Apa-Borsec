#include "Fire.hlsli"

Texture1D ObjRandomTexture : register( t0 );
SamplerState ObjWrapSampler : register( s0 );


struct GSIn
{
	float4 ParticlePos : SV_POSITION;
	float3 ParticleDir : DIRECTION;

	float2 Size : SIZE;

	float Age : AGE;
	unsigned int Type : TYPE;
};


struct GSOut
{
	float3 ParticlePos : POSITION;
	float3 ParticleDir : DIRECTION;

	float2 Size : SIZE;

	float Age : AGE;
	unsigned int Type : TYPE;
};


float3 GetRandomDirection( float offset )
{
	float u = gSystemLifeTime + offset;

	float3 dir = ObjRandomTexture.SampleLevel( ObjWrapSampler, u, 0 ).xyz;

	return normalize( dir );
}


[maxvertexcount(2)]
void main(
	point GSIn input[1] : SV_POSITION,
	inout PointStream< GSOut > output
)
{
	input[ 0 ].Age += gDeltaTime;
	if ( input[ 0 ].Type == EMITTER )
	{
		if ( input[ 0 ].Age > 0.05f )
		{

			GSOut newElem;
			newElem.ParticlePos = gEmitPosW;
			newElem.ParticleDir = GetRandomDirection( 0 );
			newElem.ParticleDir.x *= 0.5f;
			newElem.ParticleDir.z *= 0.5f;
			newElem.ParticleDir.y = abs( newElem.ParticleDir.y );
			newElem.ParticleDir *= 4;

			newElem.Size = float2( 1.0f, 1.0f );

			newElem.Age = 0.0f;
			newElem.Type = FLARE;
			
			input[ 0 ].Age = 0.0f;

			output.Append( newElem );
		}

		GSOut elem;
		elem.ParticlePos = input[ 0 ].ParticlePos.xyz;
		elem.ParticleDir = input[ 0 ].ParticleDir;

		elem.Size = input[ 0 ].Size;

		elem.Age = input[ 0 ].Age;
		elem.Type = input[ 0 ].Type;

		output.Append( elem );
	}
	else
	{
		if ( input[ 0 ].Age <= 2.0f )
		{
			GSOut elem;
			elem.ParticlePos = input[ 0 ].ParticlePos.xyz;
			elem.ParticleDir = input[ 0 ].ParticleDir;

			elem.Size = input[ 0 ].Size;

			elem.Age = input[ 0 ].Age;
			elem.Type = input[ 0 ].Type;

			output.Append( elem );
		}
	}
}
#include "FIreworks.hlsli"

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


[maxvertexcount( 8 )]
void main(
	point GSIn input[ 1 ] : SV_POSITION,
	inout PointStream< GSOut > output
	)
{
	input[ 0 ].Age += gDeltaTime;
	if ( input[ 0 ].Type == EMITTER )
	{
		if ( input[ 0 ].Age > 0.05f )
		{
			[unroll( 2 )]
			for ( int i = 0; i < 2; ++i )
			{
				GSOut newElem;
				newElem.ParticlePos = gEmitPosW;
				newElem.ParticleDir = GetRandomDirection( 0 );
				newElem.ParticleDir.y = abs( newElem.ParticleDir.y );
				newElem.ParticleDir *= 5;

				newElem.Size = float2( 3.0f, 3.0f );

				newElem.Age = 0.0f;
				newElem.Type = FIREWORK1;

				output.Append( newElem );
			}
			input[ 0 ].Age = 0.0f;
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
		input[ 0 ].ParticleDir += gAccelW * gDeltaTime;
		input[ 0 ].ParticlePos.xyz += input[ 0 ].ParticleDir * 5 * gDeltaTime;
		if ( input[ 0 ].Type == FIREWORK1 )
		{
			if ( input[ 0 ].Age <= 1.0f )
			{ // Pass through
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
				[unroll( 8 )]
				for ( int i = 0; i < 8; ++i )
				{
					GSOut newElem;
					newElem.ParticlePos = input[ 0 ].ParticlePos;
					newElem.ParticleDir = GetRandomDirection( 0 );

					newElem.Size = float2( 1.0f, 1.0f );

					newElem.Age = 0.0f;
					newElem.Type = FIREWORK2;

					output.Append( newElem );
				}
			}
		}
		else
		{
			if ( input[ 0 ].Age <= 1.0f )
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
}
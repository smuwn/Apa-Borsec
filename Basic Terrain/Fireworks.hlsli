#define EMITTER 0
#define FIREWORK1 1
#define FIREWORK2 2

static const float3 gAccelW = float3( 0.0f, -1.0f, 0.0f );

cbuffer cbPerFrame : register ( b0 )
{
	float3 gEyePosW;
	float pad;

	float3 gEmitPosW;
	float gSystemLifeTime;
	float3 gEmitDirW;
	float gDeltaTime;

	float4x4 gViewProj;
};


#define EMITTER 0
#define FLARE 1


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
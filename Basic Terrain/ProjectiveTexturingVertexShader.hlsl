

struct VSIn
{
    float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VSOut
{
	float4 Position : SV_POSITION;
	float4 ViewPosition : POSITION0;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;

	float4x4 gCameraView;
	float4x4 gCameraProjection;

	float4x4 gProjectorView;
	float4x4 gProjectorProjection;
};

VSOut main( float3 position : POSITION, float3 normal : NORMAL, float2 texCoord : TEXCOORD )
{
	VSOut output;
	
	output.Position = mul( float4( position, 1.0f ), gWorld );
	output.Position = mul( output.Position, gCameraView );
	output.Position = mul( output.Position, gCameraProjection );

	output.ViewPosition = mul( float4( position, 1.0f ), gWorld );
	output.ViewPosition = mul( output.ViewPosition, gProjectorView );
	output.ViewPosition = mul( output.ViewPosition, gProjectorProjection );
	
	output.normal = mul( normal, ( float3x3 )gWorld );

	output.texCoord = texCoord;

	return output;
}
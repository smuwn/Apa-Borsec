


cbuffer cbLight : register( b0 )
{
    float3 Direction;
    float pad;
    float4 Diffuse;
    float4 Ambient;
};

struct PSIn
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION;
    float3 NormalW : NORMAL;
};

float4 main( PSIn input ) : SV_TARGET
{
    input.NormalW = normalize( input.NormalW );
    float4 Color = float4( 0.0f, 0.36f, 0.036f, 1.0f );

    float4 Multiplier = Ambient;

    float3 LightDir = -Direction;

    float howMuchLight = dot( input.NormalW, LightDir );
    if ( howMuchLight > 0.0f )
    {
        Multiplier += Diffuse * howMuchLight;
    }

    return Color * Multiplier;
}
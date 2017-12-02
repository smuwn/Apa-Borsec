
cbuffer cbPerObject : register( b0 )
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjection;
    float4x4 gReflectView;
};

struct VSOut
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION0;
    float4 ReflectionPosition : POSITION1;
    float4 RefractionPosition : POSITION2;
    float2 Texture : TEXCOORD;

};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    pos.w = 1.0f;
    VSOut output = ( VSOut ) 0;
    output.PositionW = mul( pos, gWorld );
    output.PositionH = mul( output.PositionW, gView );
    output.PositionH = mul( output.PositionH, gProjection );

    float4x4 reflectViewProjectionWorld;
    float4x4 refractViewProjectionWorld;
    
    reflectViewProjectionWorld = mul( gReflectView, gProjection );
    reflectViewProjectionWorld = mul( gWorld, reflectViewProjectionWorld );
    output.ReflectionPosition = mul( pos, reflectViewProjectionWorld );

    refractViewProjectionWorld = mul( gView, gProjection );
    refractViewProjectionWorld = mul( gWorld, refractViewProjectionWorld );
    output.RefractionPosition = mul( pos, refractViewProjectionWorld );

    output.Texture = tex;

    return output;
}
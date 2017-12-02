
SamplerState ObjWrapSampler : register( s0 );
Texture2D ObjReflection : register( t0 );
Texture2D ObjRefraction : register( t1 );


struct PSIn
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION0;
    float4 ReflectionPosition : POSITION1;
    float4 RefractionPosition : POSITION2;
    float2 Texture : TEXCOORD;
};

float4 main( PSIn input ) : SV_TARGET
{
    float2 reflectionCoords;
    reflectionCoords.x = input.ReflectionPosition.x / input.ReflectionPosition.w / 2.0f + 0.5f;
    reflectionCoords.y = -input.ReflectionPosition.y / input.ReflectionPosition.w / 2.0f + 0.5f;
    float2 refractionCoords;
    refractionCoords.x = input.RefractionPosition.x / input.RefractionPosition.w / 2.0f + 0.5f;
    refractionCoords.y = -input.RefractionPosition.y / input.RefractionPosition.w / 2.0f + 0.5f;

    float4 refractionColor = ObjRefraction.Sample( ObjWrapSampler, refractionCoords );
    float4 reflectionColor = ObjReflection.Sample( ObjWrapSampler, reflectionCoords );

    float4 color =  lerp( reflectionColor, refractionColor, 0.4f );
    //clip( refractionColor.a - 0.2f );

    return color;
}
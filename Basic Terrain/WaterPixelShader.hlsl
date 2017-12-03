
SamplerState ObjWrapSampler : register( s0 );
Texture2D ObjReflection : register( t0 );
Texture2D ObjRefraction : register( t1 );
Texture2D ObjNormalMap : register( t2 );

struct PSIn
{
    float4 PositionH : SV_POSITION;
    float4 PositionW : POSITION0;
    float4 ReflectionPosition : POSITION1;
    float4 RefractionPosition : POSITION2;
    float3 EyeDirection : POSITION3;
    float2 Texture : TEXCOORD;
    float2 Texture1 : TEXCOORD1;
    float2 Texture2 : TEXCOORD2;
};

cbuffer cbWater : register( b0 )
{
    float4 WaterColor;
    float WaterShininess;
    float reflectRefractScale;
};

cbuffer cbPerObject : register( b1 )
{
    float waterTranslation;
    float3 lightDirection;
};

float4 main( PSIn input ) : SV_TARGET
{
    input.Texture1.y += waterTranslation;
    input.Texture2.y += waterTranslation;
    float4 normalMap1 = ObjNormalMap.Sample( ObjWrapSampler, input.Texture1 );
    float4 normalMap2 = ObjNormalMap.Sample( ObjWrapSampler, input.Texture2 );
    
	float3 normal1 = ( normalMap1.rgb * 2.0f ) - 1.0f;
    float3 normal2 = ( normalMap2.rgb * 2.0f ) - 1.0f;
    float3 normal = normalize( normal1 + normal2 );

    float2 reflectionCoords;
    reflectionCoords.x = input.ReflectionPosition.x / input.ReflectionPosition.w / 2.0f + 0.5f;
    reflectionCoords.y = -input.ReflectionPosition.y / input.ReflectionPosition.w / 2.0f + 0.5f;
    float2 refractionCoords;
    refractionCoords.x = input.RefractionPosition.x / input.RefractionPosition.w / 2.0f + 0.5f;
    refractionCoords.y = -input.RefractionPosition.y / input.RefractionPosition.w / 2.0f + 0.5f;

    reflectionCoords = reflectionCoords + ( normal.xy * reflectRefractScale );
    refractionCoords = refractionCoords + ( normal.xy * reflectRefractScale );

    float4 refractionColor = ObjRefraction.Sample( ObjWrapSampler, refractionCoords );
    float4 reflectionColor = ObjReflection.Sample( ObjWrapSampler, reflectionCoords );
    refractionColor = saturate( refractionColor * WaterColor );

    float4 color =  lerp( reflectionColor, refractionColor, 0.5f );

    float3 reflection = -reflect( lightDirection, normal );
	
    float specular = dot( normalize( input.EyeDirection ), normalize( reflection ) );
    if ( specular )
    {
        specular = pow( abs( specular ), WaterShininess );
        color += specular;
    }

    return color;
}
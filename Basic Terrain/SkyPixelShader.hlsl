
cbuffer cbPerObject
{
    float4 CenterColor;
    float4 ApexColor;
};

struct PSIn
{
    float4 SVPosition : SV_POSITION;
    float4 Position : POSITION;
};

float4 main(PSIn input) : SV_TARGET
{
    float height = input.Position.y;
	
    if ( height < 0.0f )
    {
        height = 0.0f;
    }

    return lerp( CenterColor, ApexColor, height );
}
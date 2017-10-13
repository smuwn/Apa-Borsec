
struct PSIn
{
    float4 Position : SV_POSITION;
    float4 DepthPosition : POSITION;
};

float4 main( PSIn input) : SV_TARGET
{
    return input.DepthPosition.z / input.DepthPosition.w;
}
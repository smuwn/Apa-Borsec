
Texture2D FFTTexture : register(t0);

RWTexture2D<float4> FinalDisplacement : register(u0);

static const int N = 256;

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float h = FFTTexture[DTid.xy].r;
    h = h / (N * N);
    FinalDisplacement[DTid.xy] = float4(h, h, h, 1.0f);
}
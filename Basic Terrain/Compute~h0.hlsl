
Texture2D ObjNoiseReal0 : register(t0);
Texture2D ObjNoiseImaginary0 : register(t1);
Texture2D ObjNoiseReal1 : register(t2);
Texture2D ObjNoiseImaginary1 : register(t3);

RWTexture2D<float4> ObjTildeh0k : register(u0);
RWTexture2D<float4> ObjTildeh0minusk : register(u1);

static const float PI = 3.14159265f;
static const float g = 9.81f;

cbuffer cbFFTInfo : register(b0)
{
	int N; // Size - 256
	int L; // Horizontal size - 1000
	float A; // Amplitude - 20
	float2 windDirection; // (1,0)
	float windSpeed; // 26
	float2 pad;
};

// Box-Muller Method
float4 Random(uint3 coords)
{
	float4 re0, im0, re1, im1;
	re0 = ObjNoiseReal0.Load(coords);
	im0 = ObjNoiseImaginary0.Load(coords);
	re1 = ObjNoiseReal1.Load(coords);
	im1 = ObjNoiseImaginary1.Load(coords);

	float noise00 = clamp(re0.r, 0.001f, 1.0f);
	float noise01 = clamp(im0.r, 0.001f, 1.0f);
	float noise02 = clamp(re1.r, 0.001f, 1.0f);
	float noise03 = clamp(im1.r, 0.001f, 1.0f);

	float u0 = 2.0f * PI * noise00;
	float v0 = sqrt(-2.0f * log(noise01));
	float u1 = 2.0f * PI * noise02;
	float v1 = sqrt(-2.0f * log(noise03));

	float4 rnd = float4(v0 * cos(u0), v0*sin(u0), v1 * cos(u1), v1*sin(u1));

	return rnd;
}

float Ph(float k, float2 kDir, float L_)
{
	return A / (k*k*k*k) * pow(dot(normalize(kDir), normalize(windDirection)), 2) * exp(-1.0f / pow(k * L_, 2)) * exp(-(k*k) * pow(L / 2000.0f, 2.0f));
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float4 rnd = Random(DTid);

	float3 x = DTid - N / 2.0f;

	float2 k = float2(2.0f * PI * x.x / L, 2.0f * PI * x.y / L);
	float L_ = (windSpeed * windSpeed) / g;
	float mag = length(k);
	if (mag < 0.00001)
		mag = 0.00001;
	float magSq = mag * mag;

	// sqrt(Ph(k)) / sqrt(2)
	float h0k = clamp(sqrt(Ph(mag, k, L_)) / sqrt(2), -4000.0, 4000.0);

	// sqrt(Ph(-k)) / sqrt(2)
	float h0minusk = clamp(sqrt(Ph(mag, -k, L_)) / sqrt(2), -4000.0, 4000.0);

	ObjTildeh0k[DTid.xy] = float4(rnd.xy * h0k, 0.0f, 1.0f);

	ObjTildeh0minusk[DTid.xy] = float4(rnd.zw * h0minusk, 0.0f, 1.0f);
}
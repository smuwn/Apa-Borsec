
static const float PI = 3.14159265f;
static const float g = 9.81f;

static const int N = 256;
cbuffer cbIndices : register(b0)
{
	float indices[N];
}

RWTexture2D<float4> ObjResult : register(u0);

class Complex
{
	float real;
	float imaginary;
};

int mod(uint x, uint y)
{
	return x - y * floor(x / y);
}

[numthreads(2, 512, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float k = DTid.y * mod(float(N) / pow(2, DTid.x + 1), N);
	Complex twiddle;
	twiddle.real = cos(2.0f * PI * k / float(N));
	twiddle.imaginary = sin(2.0f * PI * k / float(N));

	int butterflyspan = int(pow(2, DTid.x));
	int butterflywing;
	if (mod(DTid.y, pow(2, DTid.x + 1)) < pow(2, DTid.x))
		butterflywing = 1;
	else
		butterflywing = 0;

	if (DTid.x == 0)
	{
		// top butterfly wing
		if (butterflywing == 1)
			ObjResult[DTid.xy] = float4(twiddle.real, twiddle.imaginary, indices[DTid.x], indices[DTid.y + 1]);
		// bottom butterfly wing
		else
			ObjResult[DTid.xy] = float4(twiddle.real, twiddle.imaginary, indices[DTid.x], indices[DTid.y - 1]);
	}
	else
	{
		// top butterfly wing
		if (butterflywing == 1)
			ObjResult[DTid.xy] = float4(twiddle.real, twiddle.imaginary, DTid.y, DTid.y + butterflyspan);
		// bottom butterfly wing
		else
			ObjResult[DTid.xy] = float4(twiddle.real, twiddle.imaginary, DTid.y - butterflyspan, DTid.y);
	}
}
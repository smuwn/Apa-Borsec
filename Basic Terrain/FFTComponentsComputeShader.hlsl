

Texture2D ObjH0K : register(t0);
Texture2D ObjH0MinusK : register(t1);

RWTexture2D<float4> ObjXDisplacement : register(u0);
RWTexture2D<float4> ObjYDisplacement : register(u1);
RWTexture2D<float4> ObjZDisplacement : register(u2);

static const float PI = 3.14159265f;
static const float g = 9.81f;

cbuffer cbInfo : register(b0)
{
	int N;
	int L;
	float t;
};

class Complex
{
	float real;
	float imaginary;
};

Complex mul(Complex c0, Complex c1)
{
	//(a+bi) * (c+di) = ac + adi + bci - bd -> (ac + bd) + i(ad+bc)
	Complex c;
	c.real = c0.real * c1.real - c0.imaginary * c1.imaginary;
	c.imaginary = c0.real * c1.imaginary + c0.imaginary * c1.real;
	return c;
}

Complex add(Complex c0, Complex c1)
{
	// (a+bi) + (c+di) = (a+c) + i(b+d)
	Complex c;
	c.real = c0.real + c1.real;
	c.imaginary = c0.imaginary + c1.imaginary;
	return c;
}

Complex conj(Complex c)
{
	// (a+bi) conj = (a-bi)
	Complex ret;
	ret.real = c.real;
	ret.imaginary = -c.imaginary;
	return ret;
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

	float3 x = DTid - N / 2.0f;

	float2 k = float2(2.0f * PI * x.x / L, 2.0f * PI * x.y / L);

	float magnitude = length(k);
	if (magnitude < 0.00001)
		magnitude = 0.00001;

	float w = sqrt(g * magnitude);

	float4 h0k = ObjH0K.Load(DTid.xyz);
	float4 h0minusk = ObjH0MinusK.Load(DTid.xyz);

	Complex fourier_amp;// = Complex(h0k.r, h0k.g);
	fourier_amp.real = h0k.r;
	fourier_amp.imaginary = h0k.g;

	Complex fourier_amp_conj;// = Complex(h0minuk.r, h0minusk.g);
	fourier_amp_conj.real = h0minusk.r;
	fourier_amp_conj.imaginary = h0minusk.g;
	fourier_amp_conj = conj(fourier_amp_conj);

	float cosinus = cos(w * t);
	float sinus = sin(w * t);

	// Euler formula
	Complex exp_iwt;// = Complex(cosinus, sinus);
	exp_iwt.real = cosinus;
	exp_iwt.imaginary = sinus;
	Complex exp_iwt_inv;// = Complex(cosinus, -sinus);
	exp_iwt_inv.real = cosinus;
	exp_iwt_inv.imaginary = -sinus;

	// Dy
	Complex hkt_dy = add(mul(fourier_amp, exp_iwt), mul(fourier_amp_conj, exp_iwt_inv));

	// Dx
	Complex dx;// = Complex(0.0f, -k.x / magnitude);
	dx.real = 0.0f;
	dx.imaginary = -k.x / magnitude;
	Complex hkt_dx = mul(dx, hkt_dy);

	// Dz
	Complex dy;// = Complex(0.0f, -k.y / magnitude);
	dy.real = 0.0f;
	dy.imaginary = -k.y / magnitude;
	Complex hkt_dz = mul(dy, hkt_dy);

	ObjXDisplacement[DTid.xy] = float4(hkt_dx.real, hkt_dx.imaginary, 0.0f, 1.0f);
	ObjYDisplacement[DTid.xy] = float4(hkt_dy.real, hkt_dy.imaginary, 0.0f, 1.0f);
	ObjZDisplacement[DTid.xy] = float4(hkt_dz.real, hkt_dz.imaginary, 0.0f, 1.0f);
	
}
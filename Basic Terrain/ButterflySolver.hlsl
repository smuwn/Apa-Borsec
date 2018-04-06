
Texture2D twiddleIndices : register(t0);

Texture2D pingpong0 : register(t1);

RWTexture2D<float4> pingpong1 : register(u0);


cbuffer cbInfo : register(b0)
{
	int stage;
	int direction;
	float2 pad;
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

void horizontalButterfly(uint3 DTid)
{
    Complex H;

    float4 indices = twiddleIndices[int2(stage, DTid.x)];
    float2 p_ = pingpong0[int2(indices.z, DTid.y)].rg;
    float2 q_ = pingpong0[int2(indices.w, DTid.y)].rg;
    float2 w_ = float2(indices.x, indices.z);

    Complex p, q, w;

    p.real = p_.x;
    p.imaginary = p_.y;

    q.real = q_.x;
    q.imaginary = q_.y;

    w.real = w_.x;
    w.imaginary = w_.y;

    H = add(p, mul(w, q));

    pingpong1[DTid.xy] = float4(H.real, H.imaginary, 0.0f, 1.0f);
}

void verticalButterfly(uint3 DTid)
{
    Complex H;

    float4 indices = twiddleIndices[int2(stage, DTid.y)];
    float2 p_ = pingpong0[int2(DTid.x, indices.z)].rg;
    float2 q_ = pingpong0[int2(DTid.x, indices.w)].rg;
    float2 w_ = float2(indices.x, indices.z);

    Complex p, q, w;

    p.real = p_.x;
    p.imaginary = p_.y;

    q.real = q_.x;
    q.imaginary = q_.y;

    w.real = w_.x;
    w.imaginary = w_.y;

    H = add(p, mul(w, q));

    pingpong1[DTid.xy] = float4(H.real, H.imaginary, 0.0f, 1.0f);
}

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (direction == 0)
        horizontalButterfly(DTid);
    else if (direction == 1)
        verticalButterfly(DTid);
}
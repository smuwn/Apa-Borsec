
cbuffer cbPerObject : register( b0 )
{
    float4x4 WVP;
};


struct VSOut
{
    float4 Position : SV_POSITION;
};


VSOut main( float4 pos : POSITION )
{
    VSOut output = ( VSOut ) 0;
    output.Position = mul( pos, WVP );
	return output;
}
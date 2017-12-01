
cbuffer cbPerObject : register( b0 )
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjetion;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float2 Texture : TEXCOORD;
};

VSOut main( float4 pos : POSITION, float2 tex : TEXCOORD )
{
    VSOut output = ( VSOut ) 0;
    output.Position = mul( pos, gWorld );
    output.Position = mul( output.Position, gView );
    output.Position = mul( output.Position, gProjetion );

    output.Texture = tex;

    return output;
}
#pragma pack_matrix(row_major )

cbuffer cbPerObject : register( b0 )
{
    float4x4 matrMVP;
    float4x4 matrYUV2RGB;
    float4 textureOffset;
}

Texture2D    g_txVideoY : register( t0 );
Texture2D    g_txVideoUV : register( t1 );
SamplerState g_samLinear : register( s0 );


struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TextureUV    : TEXCOORD0;
};

float4 ps_main( VS_OUTPUT In ) : SV_TARGET
{ 
	float y = g_txVideoY.Sample(g_samLinear, In.TextureUV * textureOffset).r;
	float2 uv = g_txVideoUV.Sample(g_samLinear, In.TextureUV * textureOffset.xy).rg;
	return float4(saturate(mul(float4(y, uv, 1), matrYUV2RGB)).rgb, 1);
}

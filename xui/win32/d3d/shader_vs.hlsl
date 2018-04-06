#pragma pack_matrix(row_major )

cbuffer cbPerObject : register(b0)
{
    float4x4 matrMVP;
    float4x4 matrYUV2RGB;
}

struct VS_INPUT
{
    float4 Position     : POSITION;
    float2 TextureUV    : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float2 TextureUV    : TEXCOORD0;
};

VS_OUTPUT vs_main( VS_INPUT input )
{
    VS_OUTPUT Output;
    Output.Position = mul(input.Position, matrMVP);
    Output.TextureUV = input.TextureUV; 
    return Output;    
}

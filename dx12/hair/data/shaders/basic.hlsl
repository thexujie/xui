//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma pack_matrix(row_major)

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 transform;
    float2 tessFactor;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    VSOutput result;
    result.position = mul(position, transform);
    result.uv = uv;

    return result;
}


float4 PSMain(VSOutput input) : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv);
    //return float4(input.position.xyz, 1);
    return float4(0, 0, 0, 1);

}

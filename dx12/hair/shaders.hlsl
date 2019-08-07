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

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct HSOutput
{
    float3 position : SEM_POSITION;
    float2 uv : TEXCOORD;
};

struct HSConstOutput
{
    float factors[2]: SV_TessFactor;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 transform;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

VSOutput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    VSOutput result;

    result.position = position;
    result.uv = uv;

    return result;
}

HSConstOutput HSConst()
{
    HSConstOutput output;
    output.factors[0] = 1.0f;
    output.factors[1] = 0.0f;
    return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(4)]
[patchconstantfunc("HSConst")]
HSOutput HSMain(InputPatch<VSOutput, 4> input, uint id : SV_OutputControlPointID)
{
    HSOutput output;
    output.position = input[id].position.xyz;
    output.uv = input[id].uv;
    return output;
}

[domain("isoline")]
DSOutput DSMain(HSConstOutput input, OutputPatch<HSOutput, 4> op, float2 uv : SV_DomainLocation)
{
    DSOutput output;
    float t = uv.x;
    float3 pos = pow(1.0f - t, 3.0f) * op[0].position + 3.0f * pow(1.0f - t, 2.0f) * t * op[1].position + 3.0f * (1.0f - t) * pow(t, 2.0f) * op[2].position + pow(t, 3.0f) * op[3].position;

    output.position = float4(pos, 1.0f);
    output.uv = float2(0.0f, 0.0f);
    return output;
}


float4 PSMain(DSOutput input) : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv);
    return float4(0, 0, 0, 1);

}

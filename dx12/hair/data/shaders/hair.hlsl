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
    float2 _unused;
    float3 eyePosition;
};

struct VSOutput
{
    uint4 pointIndex : PATCH_INDEX;
};

struct HSOutput
{
    uint4 pointIndex : PATCH_INDEX;
};

struct HSConstOutput
{
    float factors[2]: SV_TessFactor;
};

struct DSOutput
{
    float4 position : SV_POSITION;
    float4 tangent : TANGENT;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float4 tangent : TANGENT;
};


Buffer<float4> g_positions : register(t0);
Buffer<float4> g_tangents: register(t1);

VSOutput VSMain(uint4 pointIndex : PATCH_INDEX)
{
    VSOutput result;
    result.pointIndex = pointIndex;
    return result;
}

HSConstOutput HSConst()
{
    HSConstOutput output;
    output.factors[0] = tessFactor.x;
    output.factors[1] = tessFactor.y;
    return output;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(1)]
[patchconstantfunc("HSConst")]
HSOutput HSMain(InputPatch<VSOutput, 1> input, uint id : SV_OutputControlPointID)
{
    HSOutput output;
    output.pointIndex = input[id].pointIndex;
    return output;
}

// B(t) = (1-t)^2 * P0 + 2t*(1-t)*P1 + t*t*P2
//float4 EvaluateBezier2(const OutputPatch<HSOutput, 1> patchs, float t)
//{
//    float coff0 = (1 - t) * (1 - t);
//    float coff1 = 2 * t * (1 - t);
//    float coff2 = t * t;
//    return coff0 * patchs[0] + coff1 * patchs[1]. + coff2 * patchs[2].position;
//}

void EvaluateBSpline4(const uint4 pointIndex, float t, out float4 position, out float4 tangent)
{
    row_major float4x4 positionMatrix = float4x4
    (
        -1 / 6.0, 3 / 6.0, -3 / 6.0, 1 / 6.0,
         3 / 6.0, -6 / 6.0, 3 / 6.0, 0,
        -3 / 6.0, 0, 3 / 6.0, 0,
         1 / 6.0, 4 / 6.0, 1 / 6.0, 0
    );
    
    row_major float4x4 tangentMatrix = float4x4
    (
        -3 / 6.0, 9 / 6.0, -9 / 6.0, 3 / 6.0,
         6 / 6.0, -12 / 6.0, 6 / 6.0, 0,
        -6 / 6.0, 0, 3 / 6.0, 0,
         0 / 6.0, 0 / 6.0, 0 / 6.0, 0
    );
    
    float4 position0 = g_positions[pointIndex[0]];
    float4 position1 = g_positions[pointIndex[1]];
    float4 position2 = g_positions[pointIndex[2]];
    float4 position3 = g_positions[pointIndex[3]];
    float4 tangent0 = g_tangents[pointIndex[0]];
    float4 tangent1 = g_tangents[pointIndex[1]];
    float4 tangent2 = g_tangents[pointIndex[2]];
    float4 tangent3 = g_tangents[pointIndex[3]];

    float4 positionCoff = mul(float4(t * t * t, t * t, t, 1), positionMatrix);
    float4 tangentCoff = mul(float4(t * t * t, t * t, t, 1), tangentMatrix);

    position = position0 * positionCoff[0] + position1 * positionCoff[1] + position2 * positionCoff[2] + position3 * positionCoff[3];
    tangent = tangent0 * tangentCoff[0] + tangent1 * tangentCoff[1] + tangent2 * tangentCoff[2] + tangent3 * tangentCoff[3];
}

[domain("isoline")]
DSOutput DSMain(HSConstOutput input, OutputPatch<HSOutput, 1> patchs, float2 uv : SV_DomainLocation)
{
    DSOutput output;
    EvaluateBSpline4(patchs[0].pointIndex, uv.x, output.position, output.tangent);
    output.position.y += uv.y * 0.2;
    output.position = mul(float4(output.position.xyz, 1.0f), transform);
    return output;
}

[maxvertexcount(4)]
void GSMain(line DSOutput vertices[2], inout TriangleStream<GSOutput> stream)
{
    float width = 0.02f;
    float3 tangent = (vertices[1].position.xyz - vertices[0].position.xyz);
    float3 cameraVector = eyePosition - vertices[0].position.xyz;
    float3 sideVector = normalize(cross(cameraVector, tangent));
    float3 width0 = sideVector * 0.5 * width;
    float3 width1 = sideVector * 0.5 * width;

    GSOutput result;
    result.position = mul(float4(vertices[0].position.xyz -width0, 1.0f), transform);
    result.tangent = vertices[0].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[0].position.xyz + width0, 1.0f), transform);
    result.tangent = vertices[0].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[1].position.xyz - width1, 1.0f), transform);
    result.tangent = vertices[1].tangent;
    stream.Append(result);
    result.position = mul(float4(vertices[1].position.xyz + width1, 1.0f), transform);
    result.tangent = vertices[1].tangent;
    stream.Append(result);

    stream.RestartStrip();
}

float4 PSMain() : SV_TARGET
{
    //return g_texture.Sample(g_sampler, input.uv);
    return float4(1, 0, 0, 1);

}

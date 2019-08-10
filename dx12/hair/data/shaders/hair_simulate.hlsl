#pragma pack_matrix(row_major)

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 transform;
    float gravityStrength;
    float timeElapse;
    float angularStiffness;

    // 约束计算的积分次数，越多越精确
    int numConstraintIterations;
};

struct Vertex
{
    float4 position;
    float2 uv;
};

RWStructuredBuffer<Vertex> currPositions : register(u0);
RWStructuredBuffer<float4> prevPositions : register(u1);

Buffer<uint> strandOffsets : register(t0);
// liner length / line stiffness / anguar length/ angular stiffness
Buffer<float4> constraints : register(t1);

#define BLOCK_SIZE 64
groupshared float4 sharedPos[BLOCK_SIZE];
groupshared float4 sharedConstraints[BLOCK_SIZE];
groupshared float3 sharedForce[BLOCK_SIZE];

float4 addForcesAndIntegrate(float4 position, float4 oldPosition, float3 force, float stiffness, int globalIndex)
{
    // 根控制点保持固定
    if (position.w == 0)
        return position;
    
    float internalSpeedCoefficient = stiffness * (0.05 - 0.02) + 0.02;
    float3 velocity = (1 - internalSpeedCoefficient) * (position.xyz - oldPosition.xyz);
    
    //if velocity is very low increase the stiffness
    if (length(velocity) < 0.01)
    {
        float stiffness_clamped = clamp(stiffness * 5.0, 0, 1);
        float stiffnessIncrease = stiffness_clamped - stiffness;
        if (stiffnessIncrease > 0)
            force *= stiffnessIncrease * 6;
    }

    //if velocity is very high decrease the stiffness
    if (length(velocity) > 0.5) 
        force *= 0.5;
    
    float gravityStrength = (1 - stiffness) * (1.2 - 0.07) + 0.07;
    gravityStrength *= gravityStrength;
    
    // Gravity
    force += float3(0, -gravityStrength, 0);
    
    float4 result = float4(0, 0, 0, position.w);
    result.xyz = position.xyz + velocity.xyz + force * timeElapse * timeElapse;
    
    float4 transformedPosition = float4(mul(float4(currPositions[globalIndex].position.xyz, 1), transform).xyz, currPositions[globalIndex].position.w);
    float staticky = stiffness * 0.0475;

    if (length(result.xyz - transformedPosition.xyz) < 1.6)
        result.xyz = (1 - staticky) * result.xyz + staticky * transformedPosition.xyz;
    return result;
}

bool IsFree(float4 particle)
{
    return particle.w > 1;
}

float2 Responsiveness(float4 particle0, float4 particle1)
{
    if (IsFree(particle0))
    {
        if (IsFree(particle1))
            return float2(0.5, 0.5);
        else
            return float2(1, 0);
    }
    else
    {
        if (IsFree(particle1))
            return float2(0, 1);
        else
            return float2(0, 0);
    }
}

void DistanceConstraint(inout float4 particle0, inout float4 particle1, float targetDistance, float stiffness = 1.0)
{
    float3 delta = particle1.xyz - particle0.xyz;
    float distance = max(length(delta), 1e-7);
    float stretching = 1 - targetDistance / distance;
    delta = stretching * delta;
    float2 responsiveness = Responsiveness(particle0, particle1);
    particle0.xyz += responsiveness[0] * delta * stiffness;
    particle1.xyz -= responsiveness[1] * delta * stiffness;
}

[numthreads(BLOCK_SIZE, 1, 1)]
void CSMain(uint localIndex : SV_GroupIndex, uint3 groupId : SV_GroupID, uint3 dispatchId : SV_DispatchThreadID)
{
    uint indexBase = groupId.x > 0 ? strandOffsets[groupId.x - 1] : 0;
    uint globalIndex = indexBase + localIndex;
    uint count = strandOffsets[groupId.x] - indexBase;
    
    float4 orgPosition = float4(0, 0, 0, 0);
    if (localIndex < count)
    {
        sharedPos[localIndex] = currPositions[globalIndex].position;
        sharedConstraints[localIndex] = constraints[globalIndex];
        sharedForce[localIndex] = float3(0, 0, 0);
        orgPosition = currPositions[globalIndex].position;
    }
    GroupMemoryBarrierWithGroupSync();
    // todo cf

    uint _half = floor(count / 2.0f);
    uint _half2 = floor((count - 1) / 2.0f);
    uint _half3 = floor((count - 3) / 2.0f);
    uint halfAng1 = max(0, _half2 - 3); //being conservative in how many particles we apply the angular constraint to
    uint halfAng2 = max(0, _half3 - 3); //being conservative in how many particles we apply the angular constraint to
    float stiffness = 0.0f;

    if (localIndex < count)
    {
        sharedPos[localIndex] = addForcesAndIntegrate(sharedPos[localIndex], prevPositions[globalIndex], sharedForce[localIndex], constraints[globalIndex].y, globalIndex);
        prevPositions[globalIndex] = orgPosition;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    for (int iteration = 0; iteration < numConstraintIterations; iteration++)
    {
        //apply distance constraints to first subset
        if (localIndex < _half)
            DistanceConstraint(sharedPos[localIndex * 2], sharedPos[localIndex * 2 + 1], sharedConstraints[localIndex * 2].x);
        
        GroupMemoryBarrierWithGroupSync();
        
        //apply distance constraints to second subset
        if (localIndex < _half2)
            DistanceConstraint(sharedPos[localIndex * 2 + 1], sharedPos[localIndex * 2 + 2], sharedConstraints[localIndex * 2 + 1].x);
        
        GroupMemoryBarrierWithGroupSync();

        int idM2 = localIndex % 2;
        int idD2 = floor(localIndex / 2.0);
        int id = idD2 * 4 + idM2;

        //apply the angular constraints to the first subset
        if (localIndex < halfAng1)
        {
            stiffness = sharedConstraints[id].w * angularStiffness;
            if (stiffness > 0)
                DistanceConstraint(sharedPos[id], sharedPos[id + 2], sharedConstraints[id].z, stiffness);
        }
        GroupMemoryBarrierWithGroupSync();

        //apply the angular constraints to the second subset
        if (localIndex < halfAng2)
        {
            id += 2;
            stiffness = sharedConstraints[id].w * angularStiffness;
            if (stiffness > 0)
                DistanceConstraint(sharedPos[id], sharedPos[id + 2], sharedConstraints[id].z, stiffness);
        }
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (localIndex < count)
    {
        currPositions[globalIndex].position = sharedPos[localIndex];
    }
}


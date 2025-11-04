struct VSInput
{
    float3 position : ATTRIB0;
    float3 normal   : ATTRIB1;
};

struct VSOutput
{
    float4 position  : SV_Position;
    float3 FragPos   : TEXCOORD0;
    float3 Normal    : TEXCOORD1;
    float3 TexCoords : TEXCOORD2;
};

struct objMatrices
{
    float4x4 u_MVP;
    float4x4 model;
    float3x3 normalModel;
};

[[vk::binding(0, 1)]]
RWStructuredBuffer<objMatrices> matrices;

[[vk::binding(1, 1)]]
RWStructuredBuffer<float4> colors;

cbuffer FragmentPositions : register(b2)
{
    float3 lightPosition;
    float3 cameraPosition;
}
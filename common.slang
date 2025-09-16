// common.slang
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

// Uniforms shared between shaders
cbuffer Globals : register(b0)
{
    float4x4 u_MVP;
    float4x4 model;
}

cbuffer FragmentGlobals : register(b1)
{
    float3 u_Light;
    float3 u_vPos;
    float4 u_Color;
    int    isTexture;
}

// cube_shader.slang
import common;

// Resources
TextureCube TextureSampler : register(t0);
SamplerState Sampler       : register(s0);

// =======================
// Vertex Shader
// =======================
VSOutput vertexMain(VSInput input)
{
    VSOutput output;

    // Clip-space position
    output.position = mul(u_MVP, float4(input.position, 1.0));

    // World-space fragment position
    output.FragPos = mul(model, float4(input.position, 1.0)).xyz;

    // Normal
    float3x3 normalMat = transpose(invert((float3x3)model));
    output.Normal = normalize(mul(normalMat, input.normal));

    // Pass texcoords (using position here)
    output.TexCoords = input.position;

    return output;
}

// =======================
// Fragment Shader
// =======================
float4 fragmentMain(VSOutput input) : SV_Target
{
    // Light properties
    float3 LightColor = float3(1.0, 1.0, 1.0);
    float3 ambient    = float3(0.2, 0.2, 0.16);

    // Diffuse
    float3 norm = normalize(input.Normal);
    float3 lightDir = normalize(u_Light - input.FragPos);
    float diff = saturate(dot(norm, lightDir));
    float3 diffuse = diff * LightColor;

    // Cubemap sample
    float4 tex = TextureSampler.Sample(Sampler, input.TexCoords);

    // Specular
    float specularStrength = 0.5;
    float3 viewDir = normalize(u_vPos - input.FragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    float3 specular = specularStrength * spec * LightColor;

    float4 result;
    if (isTexture == 1)
    {
        result = (float4(ambient, 1.0) + float4(diffuse, 1.0) + float4(specular, 1.0)) * u_Color;
    }
    else
    {
        result = ((float4(ambient, 1.0) + float4(diffuse, 1.0)) * tex + float4(specular, 1.0)) * u_Color;
    }

    result.a = u_Color.a;
    return result;
}

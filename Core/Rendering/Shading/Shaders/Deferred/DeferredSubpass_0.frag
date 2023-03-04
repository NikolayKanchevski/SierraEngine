#version 450

#include "../Types/MeshPushConstant.glsl"
#include "../Utility/ShaderDefinitions.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_UV;

layout(set = 1, binding = DIFFUSE_TEXTURE_BINDING) uniform sampler2D diffuseSampler;
layout(set = 1, binding = SPECULAR_TEXTURE_BINDING) uniform sampler2D specularSampler;
layout(set = 1, binding = NORMAL_MAP_TEXTURE_BINDING) uniform sampler2D normalSampler;

layout(location = 0) out uint toFramebuffer_ID;
layout(location = 1) out vec4 toFramebuffer_Position;
layout(location = 2) out vec4 toFramebuffer_Diffuse;
layout(location = 3) out vec2 toFramebuffer_SpecularAndShininess;
layout(location = 4) out vec4 toFramebuffer_Normal;

void main()
{
    // Read texture colors
    const vec3 diffuseTextureColor = texture(diffuseSampler, fromVert_UV).rgb;
    const float specularTextureColor = texture(specularSampler, fromVert_UV).r;
    const vec3 normalTextureColor = texture(normalSampler, fromVert_UV).rgb;

    // Calculate normal in tangent space
    const vec3 Q1  = dFdx(fromVert_Position);
    const vec3 Q2  = dFdy(fromVert_Position);
    const vec2 st1 = dFdx(fromVert_UV);
    const vec2 st2 = dFdy(fromVert_UV);
    const vec3 N = normalize(fromVert_Normal);
    const vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    const mat3 TBN = mat3(T, cross(N, T), N);
    const vec3 tnorm = TBN * normalize(normalTextureColor * 2.0 - 1.0);

    // Pass data to framebuffer
    toFramebuffer_ID = pushConstant.entityID;
    toFramebuffer_Position = vec4(fromVert_Position, 1.0f);
    toFramebuffer_Diffuse = vec4(diffuseTextureColor * pushConstant.material.diffuse, 1.0f);
    toFramebuffer_SpecularAndShininess = vec2(specularTextureColor * pushConstant.material.specular, pushConstant.material.shininess);
    toFramebuffer_Normal = vec4(tnorm, 1.0f);
}
#version 450

/* !COMPILE_TO_BINARY */
#include "../Types/MeshPushConstant.glsl"
#include "../Utility/NormalMapping.glsl"
#include "../Utility/ShaderDefinitions.glsl"

layout(location = 0) in vec3 fromVert_Position;
layout(location = 1) in vec3 fromVert_Normal;
layout(location = 2) in vec2 fromVert_UV;

const uint DIFFUSE_TEXTURE_BINDING     =    2;
const uint SPECULAR_TEXTURE_BINDING    =    3;
const uint NORMAL_TEXTURE_BINDING      =    4;
layout(binding = DIFFUSE_TEXTURE_BINDING) uniform sampler2D fromCode_DiffuseSampler;
layout(binding = SPECULAR_TEXTURE_BINDING) uniform sampler2D fromCode_SpecularSampler;
layout(binding = NORMAL_TEXTURE_BINDING) uniform sampler2D fromCode_NormalSampler;

layout(location = 0) out uint toFramebuffer_ID;
layout(location = 1) out vec4 toFramebuffer_Diffuse;
layout(location = 2) out vec2 toFramebuffer_SpecularAndShinines;
layout(location = 3) out uint toFramebuffer_Normal;

void main()
{
    // Read texture colors
    const vec3 diffuseTextureColor = texture(fromCode_DiffuseSampler, fromVert_UV).rgb;
    const float specularTextureColor = texture(fromCode_SpecularSampler, fromVert_UV).r;

    // Pass data to framebuffer
    toFramebuffer_ID = pushConstant.entityID;
    toFramebuffer_Diffuse = vec4(diffuseTextureColor * pushConstant.material.diffuse, 1.0f);
    toFramebuffer_SpecularAndShinines = vec2(specularTextureColor * pushConstant.material.specular, pushConstant.material.shininess / 512.0f);
    toFramebuffer_Normal = CompressNormal32(GetNormalInTangentSpaceFromNormalMap(fromCode_NormalSampler, fromVert_Position, fromVert_Normal, fromVert_UV));
}
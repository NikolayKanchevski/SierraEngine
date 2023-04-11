#ifndef SHADER_DEFINITIONS_GLSL
#define SHADER_DEFINITIONS_GLSL

const uint DIFFUSE_TEXTURE_OFFSET = 0;
const uint SPECULAR_TEXTURE_OFFSET = 1;
const uint NORMAL_MAP_TEXTURE_OFFSET = 3;
const uint HEIGHT_MAP_TEXTURE_OFFSET = 3;
const uint TOTAL_TEXTURE_TYPES_COUNT = 4;

const uint UNIFORM_BUFFER_BINDING = 0;
const uint STORAGE_BUFFER_BINDING = 1;
const uint DIFFUSE_TEXTURE_BINDING = 2;
const uint SPECULAR_TEXTURE_BINDING = 3;
const uint NORMAL_MAP_TEXTURE_BINDING = 4;
const uint HEIGHT_MAP_TEXTURE_BINDING = 5;

#define SET_PUSH_CONSTANT(TYPE)                                 \
    layout(push_constant) uniform _SIERRA_PUSH_CONSTANT         \
    {                                                           \
        TYPE pushConstant;                                      \
    } _sierraPushConstant

#define PUSH_CONSTANT _sierraPushConstant.pushConstant

#endif
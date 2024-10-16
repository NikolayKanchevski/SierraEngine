#ifndef SIERRA_GLSL
#define SIERRA_GLSL

#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_shader_image_load_formatted : require
#extension GL_EXT_samplerless_texture_functions : require
#extension GL_EXT_shader_explicit_arithmetic_types : require

#define int32 int
#define int32vec2 ivec2
#define int32vec3 ivec3
#define int32vec4 ivec4

#define int64 int64_t
#define int64vec2 i64vec2
#define int64vec3 i64vec3
#define int64vec4 i64vec4

#define uint32 uint
#define uint32vec2 uvec2
#define uint32vec3 uvec3
#define uint32vec4 uvec4

#define uint64 uint64_t
#define uint64vec2 u64vec2
#define uint64vec3 u64vec3
#define uint64vec4 u64vec4

#define float32 float
#define float32vec2 vec2
#define float32vec3 vec3
#define float32vec4 vec4

#define float32mat2x2 mat2x2
#define float32mat2x3 mat2x3
#define float32mat2x4 mat2x4
#define float32mat3x2 mat3x2
#define float32mat3x3 mat3x3
#define float32mat3x4 mat3x4
#define float32mat4x2 mat4x2
#define float32mat4x3 mat4x3
#define float32mat4x4 mat4x4

#define float64 double
#define float64vec2 dvec2
#define float64vec3 dvec3
#define float64vec4 dvec4

const uint32 SIERRA_RESERVED_SET                    = 0;
const uint32 SIERRA_UNIFORM_BUFFER_BINDING          = 0;
const uint32 SIERRA_STORAGE_BUFFER_BINDING          = 1;
const uint32 SIERRA_SAMPLED_IMAGE_BINDING           = 2;
const uint32 SIERRA_STORAGE_IMAGE_BINDING           = 3;
const uint32 SIERRA_SAMPLER_BINDING                 = 4;

#define SIERRA_PUSH_CONSTANT(NAME, TYPE)                                                                                                                                                \
    layout(push_constant) uniform _sierra_push_constant_##TYPE                                                                                                                          \
    {                                                                                                                                                                                   \
        TYPE value;                                                                                                                                                                     \
    } _sierra_##NAME

#define SIERRA_UNIFORM_BUFFER(NAME, TYPE, INDEX)                                                                                                                                        \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_UNIFORM_BUFFER_BINDING) uniform _sierra_uniform_buffer_##TYPE                                                                    \
    {                                                                                                                                                                                   \
        TYPE value;                                                                                                                                                                     \
    } _sierra_##NAME[]

#define SIERRA_STORAGE_BUFFER(NAME, TYPE, INDEX)                                                                                                                                        \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_BUFFER_BINDING) buffer _sierra_storage_buffer_##TYPE                                                                     \
    {                                                                                                                                                                                   \
        TYPE value;                                                                                                                                                                     \
    } _sierra_##NAME[]

#define SIERRA_READ_ONLY_STORAGE_BUFFER(NAME, TYPE, INDEX)                                                                                                                              \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_BUFFER_BINDING) readonly buffer _sierra_storage_buffer_##TYPE                                                            \
    {                                                                                                                                                                                   \
        TYPE value;                                                                                                                                                                     \
    } _sierra_##NAME[]

#define SIERRA_WRITE_ONLY_STORAGE_BUFFER(NAME, TYPE, INDEX)                                                                                                                             \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_BUFFER_BINDING) writeonly buffer _sierra_storage_buffer_##TYPE                                                           \
    {                                                                                                                                                                                   \
        TYPE value;                                                                                                                                                                     \
    } _sierra_##NAME[]

#define SIERRA_SAMPLED_IMAGE_1D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_SAMPLED_IMAGE_BINDING) uniform texture1D _sierra_##NAME[]

#define SIERRA_SAMPLED_IMAGE_2D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_SAMPLED_IMAGE_BINDING) uniform texture2D _sierra_##NAME[]

#define SIERRA_SAMPLED_IMAGE_3D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_SAMPLED_IMAGE_BINDING) uniform texture3D _sierra_##NAME[]

#define SIERRA_SAMPLED_IMAGE_CUBE(NAME, INDEX)                                                                                                                                          \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_SAMPLED_IMAGE_BINDING) uniform textureCube _sierra_##NAME[]

#define SIERRA_STORAGE_IMAGE_1D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform image1D _sierra_##NAME[]

#define SIERRA_STORAGE_IMAGE_2D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform image2D _sierra_##NAME[]

#define SIERRA_STORAGE_IMAGE_3D(NAME, INDEX)                                                                                                                                            \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform image3D _sierra_##NAME[]

#define SIERRA_STORAGE_IMAGE_CUBE(NAME, INDEX)                                                                                                                                          \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform imageCube _sierra_##NAME[]

#define SIERRA_READ_ONLY_STORAGE_IMAGE_1D(NAME, INDEX)                                                                                                                                  \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform readonly image1D _sierra_##NAME[]


#define SIERRA_READ_ONLY_STORAGE_IMAGE_2D(NAME, INDEX)                                                                                                                                  \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform readonly image2D _sierra_##NAME[]

#define SIERRA_READ_ONLY_STORAGE_IMAGE_3D(NAME, INDEX)                                                                                                                                  \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform readonly image3D _sierra_##NAME[]

#define SIERRA_READ_ONLY_STORAGE_IMAGE_CUBE(NAME, INDEX)                                                                                                                                \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform readonly imageCube _sierra_##NAME[]

#define SIERRA_WRITE_ONLY_STORAGE_IMAGE_1D(NAME, INDEX)                                                                                                                                 \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform writeonly image1D _sierra_##NAME[]

#define SIERRA_WRITE_ONLY_STORAGE_IMAGE_2D(NAME, INDEX)                                                                                                                                 \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform writeonly image2D _sierra_##NAME[]

#define SIERRA_WRITE_ONLY_STORAGE_IMAGE_3D(NAME, INDEX)                                                                                                                                 \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform writeonly image3D _sierra_##NAME[]

#define SIERRA_WRITE_ONLY_STORAGE_IMAGE_CUBE(NAME, INDEX)                                                                                                                               \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_STORAGE_IMAGE_BINDING) uniform writeonly imageCube _sierra_##NAME[]

#define SIERRA_SAMPLER(NAME, INDEX)                                                                                                                                                     \
    uint32 _sierra_index_##NAME = INDEX;                                                                                                                                                \
    layout(set = SIERRA_RESERVED_SET, binding = SIERRA_SAMPLER_BINDING) uniform sampler _sierra_##NAME[]

#define SIERRA_GET_PUSH_CONSTANT(NAME)           _sierra_##NAME.value
#define SIERRA_GET_UNIFORM_BUFFER(NAME)          _sierra_##NAME[_sierra_index_##NAME].value
#define SIERRA_GET_STORAGE_BUFFER(NAME)          _sierra_##NAME[_sierra_index_##NAME].value

#define SIERRA_GET_SAMPLED_IMAGE_1D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_SAMPLED_IMAGE_2D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_SAMPLED_IMAGE_3D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_SAMPLED_IMAGE_CUBE(NAME)      _sierra_##NAME[_sierra_index_##NAME]

#define SIERRA_GET_STORAGE_IMAGE_1D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_STORAGE_IMAGE_2D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_STORAGE_IMAGE_3D(NAME)        _sierra_##NAME[_sierra_index_##NAME]
#define SIERRA_GET_STORAGE_CUBE(NAME)            _sierra_##NAME[_sierra_index_##NAME]

#define SIERRA_GET_SAMPLER(NAME)                 _sierra_##NAME[_sierra_index_##NAME]

#endif
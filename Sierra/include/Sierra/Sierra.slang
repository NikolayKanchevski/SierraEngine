// Bindless resources of Sierra, which shaders include with: #include "Sierra/Sierra.slang"
// Every resource lives in a single descriptor set, within unbounded arrays, which are indexed by values passed through push constants ([[vk::push_constant]] ConstantBuffer<T>)

#ifndef SIERRA_SLANG
#define SIERRA_SLANG

typealias int32 = int;
typealias int64 = int64_t;
typealias uint32 = uint;
typealias uint64 = uint64_t;
typealias float32 = float;
typealias float64 = double;

// NOTE: These must match the resource table layouts of every backend (VulkanDevice.h, MetalResourceTable.h)
#define SIERRA_RESERVED_SET                 0
#define SIERRA_UNIFORM_BUFFER_BINDING       0
#define SIERRA_STORAGE_BUFFER_BINDING       1
#define SIERRA_SAMPLED_IMAGE_BINDING        2
#define SIERRA_STORAGE_IMAGE_BINDING        3
#define SIERRA_SAMPLER_BINDING              4

// Buffers can hold data of any type, so a shader declares the arrays of each type it reads, e.g. SIERRA_STORAGE_BUFFERS(ObjectData, objectBuffers), then reads objectBuffers[index][element]
#define SIERRA_UNIFORM_BUFFERS(TYPE, NAME)              [[vk::binding(SIERRA_UNIFORM_BUFFER_BINDING, SIERRA_RESERVED_SET)]] ConstantBuffer<TYPE> NAME[]
#define SIERRA_STORAGE_BUFFERS(TYPE, NAME)              [[vk::binding(SIERRA_STORAGE_BUFFER_BINDING, SIERRA_RESERVED_SET)]] RWStructuredBuffer<TYPE> NAME[]
#define SIERRA_READ_ONLY_STORAGE_BUFFERS(TYPE, NAME)    [[vk::binding(SIERRA_STORAGE_BUFFER_BINDING, SIERRA_RESERVED_SET)]] StructuredBuffer<TYPE> NAME[]

// Images and samplers are of fixed types, so they are declared once here, and retrieved with the functions below (only those a shader uses end up in it)
[[vk::binding(SIERRA_SAMPLED_IMAGE_BINDING, SIERRA_RESERVED_SET)]] Texture1D _sierraSampledImages1D[];
[[vk::binding(SIERRA_SAMPLED_IMAGE_BINDING, SIERRA_RESERVED_SET)]] Texture2D _sierraSampledImages2D[];
[[vk::binding(SIERRA_SAMPLED_IMAGE_BINDING, SIERRA_RESERVED_SET)]] Texture3D _sierraSampledImages3D[];
[[vk::binding(SIERRA_SAMPLED_IMAGE_BINDING, SIERRA_RESERVED_SET)]] TextureCube _sierraSampledImagesCube[];

// NOTE: Cube storage images are accessed as arrays of their six faces
[[vk::binding(SIERRA_STORAGE_IMAGE_BINDING, SIERRA_RESERVED_SET)]] RWTexture1D<float4> _sierraStorageImages1D[];
[[vk::binding(SIERRA_STORAGE_IMAGE_BINDING, SIERRA_RESERVED_SET)]] RWTexture2D<float4> _sierraStorageImages2D[];
[[vk::binding(SIERRA_STORAGE_IMAGE_BINDING, SIERRA_RESERVED_SET)]] RWTexture3D<float4> _sierraStorageImages3D[];
[[vk::binding(SIERRA_STORAGE_IMAGE_BINDING, SIERRA_RESERVED_SET)]] RWTexture2DArray<float4> _sierraStorageImagesCube[];

[[vk::binding(SIERRA_SAMPLER_BINDING, SIERRA_RESERVED_SET)]] SamplerState _sierraSamplers[];

namespace Sierra
{
    Texture1D GetSampledImage1D(const uint32 index) { return _sierraSampledImages1D[index]; }
    Texture2D GetSampledImage2D(const uint32 index) { return _sierraSampledImages2D[index]; }
    Texture3D GetSampledImage3D(const uint32 index) { return _sierraSampledImages3D[index]; }
    TextureCube GetSampledImageCube(const uint32 index) { return _sierraSampledImagesCube[index]; }

    RWTexture1D<float4> GetStorageImage1D(const uint32 index) { return _sierraStorageImages1D[index]; }
    RWTexture2D<float4> GetStorageImage2D(const uint32 index) { return _sierraStorageImages2D[index]; }
    RWTexture3D<float4> GetStorageImage3D(const uint32 index) { return _sierraStorageImages3D[index]; }
    RWTexture2DArray<float4> GetStorageImageCube(const uint32 index) { return _sierraStorageImagesCube[index]; }

    SamplerState GetSampler(const uint32 index) { return _sierraSamplers[index]; }
}

#endif

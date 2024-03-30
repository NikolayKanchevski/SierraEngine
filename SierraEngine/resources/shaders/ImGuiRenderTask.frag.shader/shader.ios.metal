#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PushConstant
{
    uint fontAtlasIndex;
    uint fontSamplerIndex;
    float2 translation;
    float2 scale;
};

struct _29
{
    float4 color;
    float2 UV;
};

struct spvDescriptorSetBuffer0
{
    constant void* _m0_pad [[id(0)]][250000];
    constant void* _m250000_pad [[id(250000)]][250000];
    constant void* _m500000_pad [[id(500000)]][250000];
    constant void* _m750000_pad [[id(750000)]][250000];
    array<texture2d<float>, 250000> _sierra_sampled_image_declaration [[id(1000000)]];
    array<sampler, 250000> _m1250000_pad [[id(1250000)]];
    array<texture2d<float>, 250000> _m1500000_pad [[id(1500000)]];
    array<texture2d<float>, 250000> _m1750000_pad [[id(1750000)]];
    array<sampler, 1024> _sierra_sampler_declaration [[id(2000000)]];
};

struct main0_out
{
    float4 Out [[color(0)]];
};

struct main0_in
{
    float4 In_color [[user(locn0)]];
    float2 In_UV [[user(locn1)]];
};

fragment main0_out main0(main0_in in [[stage_in]], const device spvDescriptorSetBuffer0& spvDescriptorSet0 [[buffer(0)]], constant PushConstant& pushConstant [[buffer(1)]])
{
    main0_out out = {};
    _29 In = {};
    In.color = in.In_color;
    In.UV = in.In_UV;
    out.Out = In.color * spvDescriptorSet0._sierra_sampled_image_declaration[pushConstant.fontAtlasIndex].sample(spvDescriptorSet0._sierra_sampler_declaration[pushConstant.fontSamplerIndex], In.UV).x;
    return out;
}


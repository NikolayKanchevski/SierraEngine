#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct _11
{
    float4 color;
    float2 UV;
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

fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> fontAtlas [[texture(0)]], sampler fontAtlasSmplr [[sampler(0)]])
{
    main0_out out = {};
    _11 In = {};
    In.color = in.In_color;
    In.UV = in.In_UV;
    out.Out = In.color * float4(fontAtlas.sample(fontAtlasSmplr, In.UV).x);
    return out;
}


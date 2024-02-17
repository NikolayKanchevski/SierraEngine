#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct _9
{
    float4 color;
    float2 UV;
};

struct PushConstant
{
    float2 translation;
    float2 scale;
};

struct main0_out
{
    float4 Out_color [[user(locn0)]];
    float2 Out_UV [[user(locn1)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 position [[attribute(0)]];
    float2 UV [[attribute(1)]];
    float4 color [[attribute(2)]];
};

vertex main0_out main0(main0_in in [[stage_in]], constant PushConstant& pc [[buffer(0)]])
{
    main0_out out = {};
    _9 Out = {};
    Out.color = in.color;
    Out.UV = in.UV;
    out.gl_Position = float4((in.position * pc.scale) + pc.translation, 0.0, 1.0);
    out.Out_color = Out.color;
    out.Out_UV = Out.UV;
    return out;
}


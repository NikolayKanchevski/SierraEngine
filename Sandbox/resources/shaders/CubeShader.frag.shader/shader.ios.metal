#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct _11
{
    float3 color;
};

struct main0_out
{
    float4 Out [[color(0)]];
};

struct main0_in
{
    float3 In_color [[user(locn0)]];
};

fragment main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
    _11 In = {};
    In.color = in.In_color;
    out.Out = float4(In.color, 1.0);
    return out;
}


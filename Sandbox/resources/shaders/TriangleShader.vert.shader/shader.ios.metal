#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

constant float2 _19[3] = { float2(-0.5), float2(0.5, -0.5), float2(0.0, 0.5) };
constant float3 _28[3] = { float3(1.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), float3(0.0, 0.0, 1.0) };

struct main0_out
{
    float3 m_49 [[user(locn0)]];
    float4 gl_Position [[position]];
};

vertex main0_out main0(uint gl_VertexIndex [[vertex_id]])
{
    main0_out out = {};
    out.gl_Position = float4(_19[int(gl_VertexIndex)], 0.0, 1.0);
    out.m_49 = _28[int(gl_VertexIndex)];
    return out;
}


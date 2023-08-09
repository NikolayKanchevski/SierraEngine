#ifndef NORMAL_MAPPING_GLSL
#define NORMAL_MAPPING_GLSL

#include "../Utility/Math.glsl"

vec3 GetNormalInTangentSpaceFromNormalMap(sampler2D normalMap, vec3 fragmentPosition, vec3 fragmentNormal, vec2 UV)
{
    const vec3 Q1 = dFdx(fragmentPosition);
    const vec3 Q2 = dFdy(fragmentPosition);
    const vec2 st1 = dFdx(UV);
    const vec2 st2 = dFdy(UV);
    const vec3 N = normalize(fragmentNormal);
    const vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    const mat3x3 TBN = mat3x3(T, cross(N, T), N);
    return TBN * normalize(texture(normalMap, UV).rgb * 2.0 - 1.0);
}

uint CompressNormal32(vec3 normal) {
    uint zSign = uint(normal.z < 0.0);
    float x = floor(normal.x * 32768.0) + 32767.0;
    float y = floor(normal.y * 16384.0) + 16383.0;
    return (uint(x) << 16u) | (uint(y) << 1u) | zSign;
}

vec3 DecompressNormal32(uint compressedNormal) {
    uint zSign = compressedNormal & 0x01u;
    uint xS = (compressedNormal & 0xFFFF0000u) >> 16;
    uint yS = (compressedNormal & 0x0000FFFEu) >> 1;
    float x = (float(xS) - 32767.0) / 32768.0;
    float y = (float(yS) - 16383.0) / 16384.0;
    float z = (zSign == 1 ? -1.0 : 1.0) * sqrt(1.0 - min(x * x + y * y, 1.0));
    return normalize(vec3(x, y, z));
}

#endif
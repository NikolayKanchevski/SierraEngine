#ifndef MATH_GLSL
#define MATH_GLSL

const float PI = 3.141592653589793238462643383279502884197;
const float TWO_PI = PI * 2;

float Random(vec3 seed, int i)
{
    vec4 seedVector = vec4(seed, i);
    float dot = dot(seedVector, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot) * 43758.5453);
}

float Linstep(float min_, float max_, float alpha)
{
    return clamp((alpha - min_) / (max_ - min_), 0.0, 1.0);
}

float Clipstep(float min_, float max_, float alpha)
{
    return max((alpha - min_) * (max_ / (1.0 - min_)), 0.0);
}

float Normalize(float value)
{
    return clamp(value, 0.0, 1.0);
}

vec4 Lerp(vec4 a, vec4 b, float t)
{
    return a + (b - a) * t;
}

#endif
#ifndef MATH_GLSL
#define MATH_GLSL

const float PI = 3.141592653589793238462643383279502884197;
const float TWO_PI = PI * 2;

const vec2 poissonDisk[16] = vec2[](
    vec2( -0.942016240, -0.39906216  ),
    vec2(  0.945586090, -0.76890725  ),
    vec2( -0.094184101, -0.92938870  ),
    vec2(  0.344959380,  0.29387760  ),
    vec2( -0.915885810,  0.45771432  ),
    vec2( -0.815442320, -0.87912464  ),
    vec2( -0.382775430,  0.27676845  ),
    vec2(  0.974843980,  0.75648379  ),
    vec2(  0.443233250, -0.97511554  ),
    vec2(  0.537429810, -0.47373420  ),
    vec2( -0.264969110, -0.41893023  ),
    vec2(  0.791975140,  0.19090188  ),
    vec2( -0.241888400,  0.99706507  ),
    vec2( -0.814099550,  0.91437590  ),
    vec2(  0.199841260,  0.78641367  ),
    vec2(  0.143831610, -0.14100790  )
);

float Random(vec3 seed, int i)
{
    vec4 seedVector = vec4(seed, i);
    float dot = dot(seedVector, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot) * 43758.5453);
}

vec2 RandomDirection(vec3 seed, int i)
{
    return normalize(vec2(Random(seed, i), Random(seed, i + 1)));
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

uint ScaleFloatToUint(float floatValue, uint bits)
{
    return uint(floor(floatValue * pow(2, (bits - 1))) + pow(2, (bits - 1)) - 1);
}

#endif
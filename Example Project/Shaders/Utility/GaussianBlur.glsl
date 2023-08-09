#ifndef GAUSSIAN_BLUR
#define GAUSSIAN_BLUR

#include "../Utility/Math.glsl"

// Special thanks to https://github.com/Jam3/glsl-fast-gaussian-blur!

const uint BLUR_DIRECTION_HORIZONTAL = 0;
const uint BLUR_DIRECTION_VERTICAL = 1;

vec4 GaussianBlurTexel5x5(sampler2D image, vec2 UV, uint blurDirection)
{
    vec4 color = vec4(0.0);

    const vec2 direction = blurDirection == BLUR_DIRECTION_HORIZONTAL ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    const float resolution = blurDirection == BLUR_DIRECTION_HORIZONTAL ? textureSize(image, 0).x : textureSize(image, 0).y;

    const vec2 offset1 = vec2(1.3333333333333333) * direction;

    color += texture(image, UV) * 0.29411764705882354;
    color += texture(image, UV + (offset1 / resolution)) * 0.35294117647058826;
    color += texture(image, UV - (offset1 / resolution)) * 0.35294117647058826;

    return color;
}

vec4 GaussianBlurTexel9x9(sampler2D image, vec2 UV, uint blurDirection)
{
    vec4 color = vec4(0.0);

    const vec2 direction = blurDirection == BLUR_DIRECTION_HORIZONTAL ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    const float resolution = blurDirection == BLUR_DIRECTION_HORIZONTAL ? textureSize(image, 0).x : textureSize(image, 0).y;

    const vec2 offset1 = vec2(1.3846153846) * direction;
    const vec2 offset2 = vec2(3.2307692308) * direction;

    color += texture(image, UV) * 0.2270270270;
    color += texture(image, UV + (offset1 / resolution)) * 0.3162162162;
    color += texture(image, UV - (offset1 / resolution)) * 0.3162162162;
    color += texture(image, UV + (offset2 / resolution)) * 0.0702702703;
    color += texture(image, UV - (offset2 / resolution)) * 0.0702702703;

    return color;
}

vec4 GaussianBlurTexel13x13(sampler2D image, vec2 UV, uint blurDirection)
{
    vec4 color = vec4(0.0);

    const vec2 direction = blurDirection == BLUR_DIRECTION_HORIZONTAL ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    const float resolution = blurDirection == BLUR_DIRECTION_HORIZONTAL ? textureSize(image, 0).x : textureSize(image, 0).y;

    const vec2 offset1 = vec2(1.411764705882353) * direction;
    const vec2 offset2 = vec2(3.2941176470588234) * direction;
    const vec2 offset3 = vec2(5.176470588235294) * direction;

    color += texture(image, UV) * 0.1964825501511404;
    color += texture(image, UV + (offset1 / resolution)) * 0.2969069646728344;
    color += texture(image, UV - (offset1 / resolution)) * 0.2969069646728344;
    color += texture(image, UV + (offset2 / resolution)) * 0.09447039785044732;
    color += texture(image, UV - (offset2 / resolution)) * 0.09447039785044732;
    color += texture(image, UV + (offset3 / resolution)) * 0.010381362401148057;
    color += texture(image, UV - (offset3 / resolution)) * 0.010381362401148057;

    return color;
}

#endif
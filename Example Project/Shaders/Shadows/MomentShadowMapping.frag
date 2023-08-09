#version 450

layout(location = 0) out vec4 toFramebuffer_Color;

void main()
{
    const float depth = gl_FragCoord.z;
    const float squared = depth * depth;

    const vec4 moments = vec4(depth, squared, squared * depth, squared * squared);
    vec4 optimized = mat4x4(
                         -2.07224649f, 13.7948857237f, 0.105877704f, 9.7924062118f,
                          32.23703778f, -59.4683975703f, -1.9077466311f, -33.7652110555f,
                         -68.571074599f,  82.0359750338f,  9.3496555107f,  47.9456096605f,
                          39.3703274134f,-35.364903257f,  -6.6543490743f, -23.9728048165f
                    ) * moments;
    optimized[0] += 0.035955884801f;

    toFramebuffer_Color = optimized;
}

#version 450

layout(location = 0) in vec3 fromCode_VertexPosition;
layout(location = 0) out vec3 toFrag_Color;

vec3 colors[] = {
    vec3(0.583f,  0.771f,  0.014f),
    vec3(0.609f,  0.115f,  0.436f),
    vec3(0.327f,  0.483f,  0.844f),
    vec3(0.822f,  0.569f,  0.201f),
    vec3(0.435f,  0.602f,  0.223f),
    vec3(0.310f,  0.747f,  0.185f),
    vec3(0.597f,  0.770f,  0.761f),
    vec3(0.559f,  0.436f,  0.730f),
    vec3(0.359f,  0.583f,  0.152f),
    vec3(0.483f,  0.596f,  0.789f),
    vec3(0.559f,  0.861f,  0.639f),
    vec3(0.195f,  0.548f,  0.859f),
    vec3(0.014f,  0.184f,  0.576f),
    vec3(0.771f,  0.328f,  0.970f),
    vec3(0.406f,  0.615f,  0.116f),
    vec3(0.676f,  0.977f,  0.133f),
    vec3(0.971f,  0.572f,  0.833f),
    vec3(0.140f,  0.616f,  0.489f),
    vec3(0.997f,  0.513f,  0.064f),
    vec3(0.945f,  0.719f,  0.592f),
    vec3(0.543f,  0.021f,  0.978f),
    vec3(0.279f,  0.317f,  0.505f),
    vec3(0.167f,  0.620f,  0.077f),
    vec3(0.347f,  0.857f,  0.137f),
    vec3(0.055f,  0.953f,  0.042f),
    vec3(0.714f,  0.505f,  0.345f),
    vec3(0.783f,  0.290f,  0.734f),
    vec3(0.722f,  0.645f,  0.174f),
    vec3(0.302f,  0.455f,  0.848f),
    vec3(0.225f,  0.587f,  0.040f),
    vec3(0.517f,  0.713f,  0.338f),
    vec3(0.053f,  0.959f,  0.120f),
    vec3(0.393f,  0.621f,  0.362f),
    vec3(0.673f,  0.211f,  0.457f),
    vec3(0.820f,  0.883f,  0.371f),
    vec3(0.982f,  0.099f,  0.879f)
};

layout(binding = 0) uniform readonly UniformBuffer
{
    mat4x4 view;
    mat4x4 projection;
} uniformBuffer;

layout(push_constant) uniform PushConstant
{
    mat4x4 model;
} pushConstant;

void main()
{
    gl_Position = uniformBuffer.projection * uniformBuffer.view * pushConstant.model * vec4(fromCode_VertexPosition, 1.0);
    toFrag_Color = colors[gl_VertexIndex];
}
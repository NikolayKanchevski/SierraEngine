#version 450

vec3[8] cubeVertexPositions = {
    vec3(-1, -1, -1),
    vec3(1, -1, -1),
    vec3(1, 1, -1),
    vec3(-1, 1, -1),
    vec3(-1, -1, 1),
    vec3(1, -1, 1),
    vec3(1, 1, 1),
    vec3(-1, 1, 1)
};

vec3 positions[3] = {
    vec3(0.0, -0.5, 0.0),
    vec3(0.5, 0.5, 0.0),
    vec3(-0.5, 0.5, 0.0)
};

layout(set = 0, binding = 0) uniform UniformBuffer
{
    mat4 view;
    mat4 projection;
    mat4 model;
} uniformBuffer;

layout(location = 0) out vec3 toFrag_UVW;

mat4 A(vec3 v)
{
    mat4 m = mat4(1.0);
    m[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return m;
}

void main()
{
//    gl_Position = vec4(cubeVertexPositions[gl_VertexIndex], 1.0);

    gl_Position = uniformBuffer.view * uniformBuffer.projection * vec4(cubeVertexPositions[gl_VertexIndex], 1.0);
//
//    toFrag_UVW = position;
//    toFrag_UVW.xy *= -1;

    toFrag_UVW = vec3(1.0, 0.0, 0.0);
}
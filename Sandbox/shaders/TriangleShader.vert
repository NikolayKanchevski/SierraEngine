#version 450

vec2 positions[3] = vec2[](
    vec2(-0.5f, -0.5f),
    vec2( 0.5f, -0.5f),
    vec2( 0.0f,  0.5f)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(location = 0) out struct
{
    vec3 color;
} Out;

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    Out.color = colors[gl_VertexIndex];
}
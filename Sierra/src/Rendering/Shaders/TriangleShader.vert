#version 450

layout(location = 0) out vec3 toFrag_Color;

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

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    toFrag_Color = colors[gl_VertexIndex];
}
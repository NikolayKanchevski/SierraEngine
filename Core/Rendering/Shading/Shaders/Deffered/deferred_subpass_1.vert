#version 450

layout(location = 0) out vec2 toFrag_UV;

void main()
{
    // Pass data to fragment shader
    toFrag_UV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(toFrag_UV * 2.0f - 1.0f, 0.0f, 1.0f);
}
#version 450

const vec2 vertices[6] = {
    { 1.0, -1.0 },
    { 1.0, 1.0 },
    { -1.0, -1.0 },
    { -1.0, -1.0 },
    { 1.0, 1.0 },
    { -1.0, 1.0 }
};

const vec2 textureCoordinates[6] = {
    { 1.0, 0.0 },
    { 1.0, 1.0 },
    { 0.0, 0.0 },
    { 0.0, 0.0 },
    { 1.0, 1.0 },
    { 0.0, 1.0 }
};

layout(location = 0) out vec2 toFrag_UV;

void main()
{
    // Pass data to fragment shader
    gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
    toFrag_UV = textureCoordinates[gl_VertexIndex];
}
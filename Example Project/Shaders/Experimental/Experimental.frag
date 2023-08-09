#version 450

layout(location = 0) in vec3 fromVert_Color;
layout(location = 0) out vec4 toFramebuffer_Color;

void main()
{
    // Set pixel in output image
    toFramebuffer_Color = vec4(fromVert_Color, 1.0);
}
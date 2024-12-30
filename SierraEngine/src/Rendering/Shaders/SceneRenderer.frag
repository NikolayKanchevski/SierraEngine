#version 450

#include "SceneRenderer.glsl"

in struct
{
    vec3 color;
} In;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(In.color, 1.0);
}

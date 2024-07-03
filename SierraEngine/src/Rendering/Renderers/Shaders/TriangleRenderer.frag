#version 450

layout(location = 0) in struct
{
    vec3 color;
} In;

layout(location = 0) out vec4 Out;

void main()
{
    Out = vec4(In.color, 1.0);
}
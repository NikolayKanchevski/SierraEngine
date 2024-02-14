#version 450

layout(location = 0) in struct { vec4 color; vec2 UV; } In;
layout(binding = 0) uniform sampler2D fontAtlas;

layout(location = 0) out vec4 Out;


void main()
{
    Out = In.color * vec4(texture(fontAtlas, In.UV.st).r);
}
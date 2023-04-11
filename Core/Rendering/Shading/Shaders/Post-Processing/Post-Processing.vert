#version 450

#include "../Utility/FullscreenTriangle.glsl"

layout(location = 0) out vec2 toFrag_UV;

void main()
{
    DRAW_FULLSCREEN_TRIANGLE(toFrag_UV);
}
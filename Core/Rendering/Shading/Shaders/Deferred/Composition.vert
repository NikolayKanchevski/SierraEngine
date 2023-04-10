#version 450

#include "../Utility/Cube.glsl"
#include "../Utility/FullscreenTriangle.glsl"
#include "../Types/GlobalUniformBuffer.glsl"

struct PushConstant
{
    mat4x4 skyboxModel;
    mat4x4 lightSpaceMatrix;
    uint renderedImageValue;
};

SET_PUSH_CONSTANT(PushConstant);

layout(location = 0) out uint toFrag_DrawingSkybox;
layout(location = 1) out vec3 toFrag_UVW;

void main()
{
    /*
        Shader expects to be called for exactly 39 vertices:
            '- 3x for the fullscreen triangle
            '- 36x more for the skybox cube
    */

    // Check wether fullscreen triangle has been drawn from the current vertex index
    const bool drawingSkybox = gl_VertexIndex >= 3;
    toFrag_DrawingSkybox = uint(drawingSkybox);

    if (drawingSkybox)
    {
        // Get current cube vertex position from cube vertices array in Cube.glsl
        vec3 vertexPosition = cubeVertices[gl_VertexIndex - 3];

        // Set final world position for vertex
        gl_Position = (uniformBuffer.projection * mat4x4(mat3x3(uniformBuffer.view)) * PUSH_CONSTANT.skyboxModel * vec4(vertexPosition, 1.0)).xyww;

        // Send texture coordinates for skubox cubemap
        toFrag_UVW = vertexPosition;
        toFrag_UVW.y *= -1;
    }
    else
    {
        // Draw fullscreen triangle
        DRAW_FULLSCREEN_TRIANGLE(toFrag_UVW);
    }
}
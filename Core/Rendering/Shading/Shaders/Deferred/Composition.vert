#version 450

#include "../Utility/Cube.glsl"
#include "../Types/GlobalUniformBuffer.glsl"

const vec2 quadVertices[6] = {
    {  1.0, -1.0 },
    {  1.0,  1.0 },
    { -1.0, -1.0 },
    { -1.0, -1.0 },
    {  1.0,  1.0 },
    { -1.0,  1.0 }
};

const vec2 quadUVs[6] = {
    { 1.0, 0.0 },
    { 1.0, 1.0 },
    { 0.0, 0.0 },
    { 0.0, 0.0 },
    { 1.0, 1.0 },
    { 0.0, 1.0 }
};

layout(push_constant) uniform FinalizationPushConstant
{
    mat4x4 skyboxModel;
    uint renderedImageValue;
} pushConstant;

layout(location = 0) out uint toFrag_DrawingSkybox;
layout(location = 1) out vec3 toFrag_UVW;

void main()
{
    /*
        Shader expects to be called for exactly 42 vertices:
            '- 6x for the fullscreen quad (consisting of 2 triangles each with 3 vertices)
            '- 36x more for the skybox cube
    */

    // Check wether fullscreen quad has been drawn from the current vertex index
    const bool drawingSkybox = gl_VertexIndex >= 6;
    toFrag_DrawingSkybox = uint(drawingSkybox);

    if (drawingSkybox)
    {
        // Get current cube vertex position from cube vertices array in Cube.glsl
        vec3 vertexPosition = cubeVertices[gl_VertexIndex - 6];

        // Set final world position for vertex
        gl_Position = (uniformBuffer.projection * mat4x4(mat3x3(uniformBuffer.view)) * pushConstant.skyboxModel * vec4(vertexPosition, 1.0)).xyww;

        // Send texture coordinates for skubox cubemap
        toFrag_UVW = vertexPosition;
        toFrag_UVW.y *= -1;
    }
    else
    {
        // Simply copy data from our quad
        gl_Position = vec4(quadVertices[gl_VertexIndex], 0.0, 1.0);
        toFrag_UVW = vec3(quadUVs[gl_VertexIndex], 0.0);
    }
}
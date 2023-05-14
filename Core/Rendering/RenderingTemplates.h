//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

#include "RenderingSettings.h"

#include "../../Engine/Components/Lighting/PointLight.h"
#include "../../Engine/Components/Lighting/DirectionalLight.h"

namespace Sierra::Core::Rendering
{
    struct Vertex
    {
        Vector3 position;
        Vector3 normal;
        Vector2 UV;
    };

    struct UniformData
    {
        Matrix4x4 view;
        Matrix4x4 projection;
        Matrix4x4 inverseView;
        Matrix4x4 inverseProjection;

        float nearPlane;
        float farPlane;
        Vector2 _align_1;
    };

    struct ObjectData
    {
        Matrix4x4 model;
    };

    struct StorageData
    {
        ObjectData objectDatas[MAX_MESHES];

        Sierra::Engine::Components::DirectionalLight::ShaderDirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
        Sierra::Engine::Components::PointLight::ShaderPointLight pointLights[MAX_POINT_LIGHTS];

        uint directionalLightCount;
        uint pointLightCount;

        float _align1_;
        float _align2_;
    };
}

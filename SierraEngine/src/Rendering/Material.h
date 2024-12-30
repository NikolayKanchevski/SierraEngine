//
// Created by Nikolay Kanchevski on 15.11.24.
//

#pragma once

namespace SierraEngine
{

    struct MaterialDiffuse
    {
        Color32 tint = { 1.0f, 1.0f, 1.0f, 1.0f };
        Sierra::SampledImageID imageID = { };
    };

    struct MaterialSpecular
    {
        float32 shininess = 1.0f;
        Sierra::SampledImageID imageID = { };
    };

    struct MaterialNormal
    {
        float32 scale = 1.0f;
        Sierra::SampledImageID imageID = { };
    };

    struct Material
    {
        MaterialDiffuse diffuse = { };
        MaterialSpecular specular = { };
        MaterialNormal normal = { };
    };

}
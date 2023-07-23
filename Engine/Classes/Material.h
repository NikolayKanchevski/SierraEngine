//
// Created by Nikolay Kanchevski on 20.07.23.
//

#pragma once

namespace Sierra::Engine
{
    struct Material
    {
        Vector3 diffuse = Vector3(1.0f, 1.0f, 1.0f);
        float specular = 1.0f;

        float shininess = 0.001953125f;
        float vertexExaggeration = 0.0f;

        Vector2 _align1_;
    };
}
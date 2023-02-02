//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once


struct Material
{
    Vector3 diffuse = Vector3(0);
    float shininess = 0.0f;

    Vector3 specular = Vector3(0);
    float vertexExaggeration = 0.0f;

    Vector3 ambient = Vector3(0);
    float _align1_;
};
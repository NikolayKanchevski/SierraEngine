//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once


struct Material
{
    glm::vec3 diffuse = glm::vec3(0);
    float shininess = 0.0f;

    glm::vec3 specular = glm::vec3(0);
    float vertexExaggeration = 0.0f;

    glm::vec3 ambient = glm::vec3(0);
    float _align1_;
};
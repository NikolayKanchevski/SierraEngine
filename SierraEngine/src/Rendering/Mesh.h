//
// Created by Nikolay Kanchevski on 11.11.26.
//

#pragma once

#include "Vertex.h"

namespace SierraEngine
{

    struct Mesh
    {
        std::vector<Vertex> vertices = { };
        std::vector<uint32> indices = { };
    };

}
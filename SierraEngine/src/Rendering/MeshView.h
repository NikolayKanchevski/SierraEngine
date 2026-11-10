//
// Created by Nikolay Kanchevski on 8.11.24.
//

#pragma once

namespace SierraEngine
{

    struct MeshView
    {
        uint32 vertexOffset = 0;
        uint32 vertexCount = 0;

        uint32 indexOffset = 0;
        uint32 indexCount = 0;
    };

}
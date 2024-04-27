//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Mesh.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(const MeshCreateInfo &createInfo)
        : vertexByteOffset(createInfo.vertexByteOffset), indexByteOffset(createInfo.indexByteOffset), vertexCount(createInfo.vertexCount), indexCount(createInfo.indexCount)
    {

    }

}
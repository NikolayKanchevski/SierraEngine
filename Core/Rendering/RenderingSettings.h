//
// Created by Nikolay Kanchevski on 9.02.23.
//

#pragma once

#define MAX_MESHES 8192
#define MAX_TEXTURES MAX_MESHES * static_cast<uint>(TextureType::TOTAL_COUNT)
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16

#define UNIFORM_BUFFER_BINDING 0
#define STORAGE_BUFFER_BINDING 1

#define VERTEX_INDEX_TYPE uint32
#define VK_VERTEX_INDEX_BUFFER_TYPE std::is_same_v<VERTEX_INDEX_TYPE, uint32> ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16

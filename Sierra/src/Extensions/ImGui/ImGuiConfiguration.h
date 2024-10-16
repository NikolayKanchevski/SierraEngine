//
// Created by Nikolay Kanchevski on 4.6.2024.
//

#pragma once

#include <cstdint>

#define IMGUI_API SIERRA_API
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#define IM_VEC2_CLASS_EXTRA                                                                           \
    ImVec2(const Vector2 other) noexcept { x = other.x; y = other.y; }                                \
    operator Vector2() const noexcept { return Vector2(x, y); }

#define IM_VEC4_CLASS_EXTRA                                                                           \
    ImVec4(const Vector4 other) noexcept { x = other.x; y = other.y; z = other.z; w = other.w; }      \
    operator Vector4() const noexcept { return Vector4(x, y, z, w); }

#define ImTextureID uint64_t
#define ImDrawIdx uint32_t
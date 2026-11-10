//
// Created by Nikolay Kanchevski on 22.09.26.
//

#pragma once

#include "../../Rendering/Mesh.h"

namespace SierraEngine
{

    struct MeshOptimizeInfo
    {
        std::span<const Vertex> vertices = { };
        std::span<const uint32> indices = { };
    };

    class SIERRA_ENGINE_API MeshOptimizer
    {
    public:

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<Mesh> Optimize(const MeshOptimizeInfo& optimizeInfo) const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        MeshOptimizer(const MeshOptimizer&) = delete;
        MeshOptimizer& operator=(const MeshOptimizer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~MeshOptimizer() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        MeshOptimizer() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        MeshOptimizer(MeshOptimizer&&) noexcept = default;
        MeshOptimizer& operator=(MeshOptimizer&&) noexcept = default;

    };

}

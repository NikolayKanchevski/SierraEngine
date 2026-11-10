//
// Created by Nikolay Kanchevski on 22.09.26.
//

#pragma once

namespace SierraEngine
{

    class ZeuxMeshOptimizer final : public MeshOptimizer
    {
    public:
        /* --- CONSTRUCTORS --- */
        ZeuxMeshOptimizer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<Mesh> Optimize(const MeshOptimizeInfo& optimizeInfo) const noexcept override;

        /* --- COPY SEMANTICS --- */
        ZeuxMeshOptimizer(const ZeuxMeshOptimizer&) = delete;
        ZeuxMeshOptimizer& operator=(const ZeuxMeshOptimizer&) = delete;

        /* --- MOVE SEMANTICS --- */
        ZeuxMeshOptimizer(ZeuxMeshOptimizer&&) noexcept = default;
        ZeuxMeshOptimizer& operator=(ZeuxMeshOptimizer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ZeuxMeshOptimizer() noexcept override = default;

    };

}

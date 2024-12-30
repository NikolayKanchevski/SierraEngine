//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

#include "../../Rendering/Mesh.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API MeshRenderer final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MeshRenderer(const Mesh& mesh) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Mesh& GetMesh() const noexcept { return mesh; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Mesh Renderer"; }

        /* --- COPY SEMANTICS --- */
        MeshRenderer(const MeshRenderer&) = delete;
        MeshRenderer& operator=(const MeshRenderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        MeshRenderer(MeshRenderer&&) noexcept = default;
        MeshRenderer& operator=(MeshRenderer&&) noexcept = default;

        /* --- DESTRUCTORS --- */
        ~MeshRenderer() noexcept = default;

    private:
        Mesh mesh = { };

    };

}
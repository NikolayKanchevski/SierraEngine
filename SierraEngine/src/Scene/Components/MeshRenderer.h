//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

#include "../Component.h"

#include "../../Rendering/MeshView.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API MeshRenderer final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MeshRenderer(const MeshView& mesh) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const MeshView& GetMesh() const noexcept { return mesh; }

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
        MeshView mesh = { };

    };

}
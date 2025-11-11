//
// Created by Nikolay Kanchevski on 28.10.25.
//

#pragma once

#include "../../../SierraEngine/src/Rendering/SceneRenderer.h"

namespace SierraEngine
{

    struct ViewportCreateInfo
    {
        RenderingContext& renderingContext;
        std::shared_ptr<SceneRenderer>& renderer;
    };

    class Viewport final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Viewport(const ViewportCreateInfo& createInfo);

        /* --- SETTER METHODS --- */
        void SetRenderer(const std::shared_ptr<SceneRenderer>& renderer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsHidden() const noexcept { return width == 0 || height == 0; }

        [[nodiscard]] uint32 GetWidth() const noexcept { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept { return height; }
        [[nodiscard]] Sierra::SampledImageID GetImageID() const noexcept { return imageID; }

        [[nodiscard]] SceneRenderer& GetRenderer() const noexcept { return *renderer; }
        [[nodiscard]] RenderTargetID GetRenderTarget() const noexcept { return renderTarget; }

        [[nodiscard]] Camera& GetCamera() noexcept { return camera; }
        [[nodiscard]] const Camera& GetCamera() const noexcept { return camera; }

        [[nodiscard]] Transform& GetTransform() noexcept { return transform; }
        [[nodiscard]] const Transform& GetTransform() const noexcept { return transform; }

        /* --- SETTER METHODS --- */
        void SetWidth(uint32 width) noexcept;
        void SetHeight(uint32 height) noexcept;

        /* --- COPY SEMANTICS --- */
        Viewport(const Viewport&) = delete;
        Viewport& operator=(const Viewport&) = delete;

        /* --- MOVE SEMANTICS --- */
        Viewport(Viewport&&) noexcept = default;
        Viewport& operator=(Viewport&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Viewport() noexcept;

    private:
        const RenderingContext* renderingContext;

        RenderTargetID renderTarget = 0;
        std::shared_ptr<SceneRenderer> renderer = nullptr;

        Camera camera = { };
        Transform transform = { };

        uint32 width = 0;
        uint32 height = 0;
        Sierra::SampledImageID imageID = { };

    };

}

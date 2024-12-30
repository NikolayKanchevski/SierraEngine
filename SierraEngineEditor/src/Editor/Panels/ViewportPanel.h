//
// Created by Nikolay Kanchevski on 24.10.24.
//

#pragma once

#include "../EditorPanel.h"

namespace SierraEngine
{

    struct ViewportCreateInfo
    {
        std::string_view title = "Viewport";
        const Sierra::Device& device;

        SceneRenderer& renderer;
        Sierra::ResourceTable& resourceTable;
    };

    class ViewportPanel final : public EditorPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ViewportPanel(const ViewportCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(bool* open = nullptr);

        /* --- SETTER METHODS --- */
        void SetRenderer(SceneRenderer& renderer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetWidth() const noexcept { return width; }
        [[nodiscard]] uint32 GetHeight() const noexcept { return height; }

        [[nodiscard]] Camera& GetCamera() noexcept { return camera; }
        [[nodiscard]] const Camera& GetCamera() const noexcept { return camera; }

        [[nodiscard]] Transform& GetTransform() noexcept { return transform; }
        [[nodiscard]] const Transform& GetTransform() const noexcept { return transform; }

        [[nodiscard]] SceneRenderer& GetRenderer() const noexcept { return *renderer; }
        [[nodiscard]] RenderTargetID GetRenderTargetID() const noexcept { return renderTargetID; }

        /* --- COPY SEMANTICS --- */
        ViewportPanel(const ViewportPanel&) = delete;
        ViewportPanel& operator=(const ViewportPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        ViewportPanel(ViewportPanel&&) noexcept = default;
        ViewportPanel& operator=(ViewportPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ViewportPanel() noexcept override;

    private:
        const Sierra::Device* device;
        std::string title = "Viewport";

        SceneRenderer* renderer = nullptr;
        RenderTargetID renderTargetID = 0;
        Sierra::SampledImageID imageID = { };

        Sierra::ResourceTable* resourceTable;

        uint32 width = 100;
        uint32 height = 100;

        Camera camera = { };
        Transform transform = { };

    };

}
//
// Created by Nikolay Kanchevski on 17.02.23.
//

#pragma once

namespace Sierra::Core::Rendering::Vulkan::Renderers { class VulkanRenderer; }

namespace Sierra::Core::Rendering::UI
{

    class UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        UIPanel() = default;

        /* --- POLLING METHODS --- */
        inline virtual void DrawUI() { };
    };

    class ViewportPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        ViewportPanel() = default;

        /* --- POLLING METHODS --- */
        void DrawUI() override;
    };

    class HierarchyPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        HierarchyPanel() = default;

        /* --- POLLING METHODS --- */
        void DrawUI() override;
    };

    class PropertiesPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        PropertiesPanel() = default;

        /* --- POLLING METHODS --- */
        void DrawUI() override;
    };

    class RendererViewportPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit RendererViewportPanel(Vulkan::Renderers::VulkanRenderer &givenRenderer) : renderer(givenRenderer) { };

        /* --- POLLING METHODS --- */
        void DrawUI() override;

    private:
        Vulkan::Renderers::VulkanRenderer &renderer;

    };

    class DebugPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit DebugPanel(Vulkan::Renderers::VulkanRenderer &givenRenderer) : renderer(givenRenderer) { };

        /* --- POLLING METHODS --- */
        void DrawUI() override;

    private:
        Vulkan::Renderers::VulkanRenderer &renderer;

    };

    class DetailedDebugPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit DetailedDebugPanel(Vulkan::Renderers::VulkanRenderer &givenRenderer) : renderer(givenRenderer) { };

        /* --- POLLING METHODS --- */
        void DrawUI() override;

    private:
        Vulkan::Renderers::VulkanRenderer &renderer;

    };

    class GamePadDebugPanel : public UIPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline GamePadDebugPanel() = default;

        /* --- POLLING METHODS --- */
        void DrawUI() override;
    };

}

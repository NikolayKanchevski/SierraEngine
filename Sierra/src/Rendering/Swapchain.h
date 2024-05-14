//
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Core/Window.h"

#include "RenderPass.h"
#include "CommandBuffer.h"

namespace Sierra
{

    class SIERRA_API SwapchainEvent : public Event { };
    template<typename T> concept SwapchainEventType = std::is_base_of_v<SwapchainEvent, T> && !std::is_same_v<SwapchainEvent, std::decay_t<T>>;

    class SIERRA_API SwapchainResizeEvent final : public SwapchainEvent
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit SwapchainResizeEvent(const uint32 width, const uint32 height, const uint32 scaling) : width(width), height(height), scaling(scaling) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetWidth() const { return width; }
        [[nodiscard]] inline uint32 GetHeight() const { return height; }
        [[nodiscard]] inline uint32 GetScaling() const { return scaling; }

    private:
        const uint32 scaling = 1;
        const uint32 width = 0;
        const uint32 height = 0;

    };

    enum class SwapchainPresentationMode : bool
    {
        Immediate,
        VSync
    };

    enum class SwapchainBuffering : bool
    {
        DoubleBuffering,
        TripleBuffering
    };

    enum class SwapchainImageMemoryType : uint8
    {
        UNorm8,
        SRGB8,
        UNorm16
    };

    struct SwapchainCreateInfo
    {
        std::string_view name = "Swapchain";
        std::unique_ptr<Window> &window;
        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::VSync;
        SwapchainBuffering preferredBuffering = !SR_PLATFORM_MOBILE ? SwapchainBuffering::TripleBuffering : SwapchainBuffering::DoubleBuffering;
        SwapchainImageMemoryType preferredImageMemoryType = SwapchainImageMemoryType::UNorm8;
    };

    class SIERRA_API Swapchain : public virtual RenderingResource
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<SwapchainEventType EventType>
        using EventCallback = std::function<bool(const EventType&)>;

        /* --- POLLING METHODS --- */
        virtual void AcquireNextImage() = 0;
        virtual void Present(std::unique_ptr<CommandBuffer> &commandBuffer) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetCurrentFrameIndex() const = 0;
        [[nodiscard]] virtual uint32 GetCurrentImageIndex() const = 0;
        [[nodiscard]] virtual uint32 GetConcurrentFrameCount() const = 0;

        [[nodiscard]] inline uint32 GetWidth() const { return GetImage(0)->GetWidth(); };
        [[nodiscard]] inline uint32 GetHeight() const { return GetImage(0)->GetHeight(); };
        [[nodiscard]] virtual uint32 GetScaling() const = 0;

        [[nodiscard]] virtual const std::unique_ptr<Image>& GetImage(uint32 frameIndex) const = 0;
        [[nodiscard]] inline const std::unique_ptr<Image>& GetCurrentImage() const { return GetImage(GetCurrentImageIndex()); };

        /* --- EVENTS --- */
        template<SwapchainEventType EventType>
        void OnEvent(const EventCallback<EventType> &Callback) { }

        /* --- OPERATORS --- */
        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Swapchain() = default;

    protected:
        explicit Swapchain(const SwapchainCreateInfo &createInfo);
        [[nodiscard]] inline EventDispatcher<SwapchainResizeEvent>& GetSwapchainResizeDispatcher() { return swapchainResizeDispatcher; };

    private:
        EventDispatcher<SwapchainResizeEvent> swapchainResizeDispatcher;

    };

    template<> inline void Swapchain::OnEvent<SwapchainResizeEvent>(const EventCallback<SwapchainResizeEvent> &Callback) { swapchainResizeDispatcher.Subscribe(Callback); }

}

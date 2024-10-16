//
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Windowing/Window.h"

#include "CommandBuffer.h"
#include "SwapchainEvents.h"

namespace Sierra
{

    template<typename T>
    concept SwapchainEventType = std::is_base_of_v<SwapchainEvent, T> && !std::is_same_v<SwapchainEvent, std::decay_t<T>>;

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
        Window& window;
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
        virtual void Present(CommandBuffer& commandBuffer) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetCurrentFrameIndex() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetCurrentImageIndex() const noexcept = 0;
        [[nodiscard]] virtual uint32 GetConcurrentFrameCount() const noexcept = 0;

        [[nodiscard]] uint32 GetWidth() const noexcept { return GetImage(0).GetWidth(); }
        [[nodiscard]] uint32 GetHeight() const noexcept { return GetImage(0).GetHeight(); }
        [[nodiscard]] virtual uint32 GetScaling() const noexcept = 0;

        [[nodiscard]] virtual const Image& GetImage(uint32 frameIndex) const = 0;
        [[nodiscard]] const Image& GetCurrentImage() const noexcept { return GetImage(GetCurrentImageIndex()); }

        /* --- EVENTS --- */
        template<SwapchainEventType EventType>
        EventSubscriptionID AddEventListener(const EventCallback<EventType>&);
        
        template<SwapchainEventType EventType>
        bool RemoveEventListener(EventSubscriptionID) noexcept;

        /* --- COPY SEMANTICS --- */
        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

        /* --- MOVE SEMANTICS --- */
        Swapchain(Swapchain&&) = delete;
        Swapchain& operator=(Swapchain&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Swapchain() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Swapchain(const SwapchainCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] EventDispatcher<SwapchainResizeEvent>& GetSwapchainResizeDispatcher() noexcept { return swapchainResizeDispatcher; }

    private:
        EventDispatcher<SwapchainResizeEvent> swapchainResizeDispatcher = { };

    };

    template<> inline EventSubscriptionID Swapchain::AddEventListener<SwapchainResizeEvent>(const EventCallback<SwapchainResizeEvent>& Callback) { return swapchainResizeDispatcher.Subscribe(Callback); }
    template<> inline bool Swapchain::RemoveEventListener<SwapchainResizeEvent>(const EventSubscriptionID ID) noexcept { return swapchainResizeDispatcher.Unsubscribe(ID); }

}

    //
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../Windowing/Screen.h"
#include "../Windowing/Window.h"
#include "../Files/FileManager.h"

namespace Sierra
{

    enum class PlatformType : uint8
    {
        Windows,
        Linux,
        macOS,
        Android,
        iOS
    };

    class SIERRA_API PlatformContext
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ScreenEnumerationPredicate = std::function<void(Screen&)>;

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const FileManager& GetFileManager() const noexcept = 0;

        [[nodiscard]] virtual Screen& GetPrimaryScreen() noexcept = 0;
        [[nodiscard]] virtual Screen& GetWindowScreen(const Window& window) = 0;
        virtual void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) = 0;

        [[nodiscard]] virtual PlatformType GetType() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        PlatformContext(const PlatformContext&) = delete;
        PlatformContext& operator=(const PlatformContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        PlatformContext(PlatformContext&&) = delete;
        PlatformContext& operator=(PlatformContext&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformContext() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        PlatformContext() noexcept = default;

        friend class Application;
        virtual void Update() { };

    };

}

//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the LinuxContext.h file is only allowed in Linux builds!"
#endif

#include "../PlatformContext.h"

#include "../../Files/Unix/UnixFileManager.h"
#include "../../Windowing/Linux/Wayland/WaylandContext.h"
#include "../../Windowing/Linux/X11/X11Context.h"

namespace Sierra
{

    class SIERRA_API LinuxContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        LinuxContext();

        /* --- GETTER METHODS --- */
        [[nodiscard]] const WindowManager& GetWindowManager() const noexcept override { return *windowManager; }
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Linux; }
        [[nodiscard]] WindowingBackendType GetWindowingBackendType() const noexcept { return windowingBackendType; }

        /* --- COPY SEMANTICS --- */
        LinuxContext(const LinuxContext&) = delete;
        LinuxContext& operator=(const LinuxContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        LinuxContext(LinuxContext&&) = delete;
        LinuxContext& operator=(LinuxContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~LinuxContext() noexcept override = default;

    private:
        // Exactly one of these is created, as the windowing backend is picked upon construction
        std::unique_ptr<WaylandContext> waylandContext = nullptr;
        std::unique_ptr<X11Context> x11Context = nullptr;

        // NOTE: Window manager references the context above, so it must be destroyed before it, which is why it is declared after
        std::unique_ptr<WindowManager> windowManager = nullptr;
        UnixFileManager fileManager;
        WindowingBackendType windowingBackendType = WindowingBackendType::X11;

        void Update() override;

    };

}

    //
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../Files/FileManager.h"
#include "../Windowing/Screen.h"
#include "../Windowing/Window.h"
#include "../Windowing/WindowManager.h"

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
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const WindowManager& GetWindowManager() const noexcept = 0;
        [[nodiscard]] virtual const FileManager& GetFileManager() const noexcept = 0;

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
        virtual void Update() = 0;

    };

}

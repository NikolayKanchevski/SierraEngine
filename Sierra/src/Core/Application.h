//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Version.hpp"
#include "WindowManager.h"
#include "PlatformContext.h"
#include "../Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        uint16 maxFrameRate = SR_PLATFORM_MOBILE * 60;
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    struct ApplicationCreateInfo
    {
        std::string_view name = "Sierra Application";
        Version version = Version({ 1, 0, 0 });
        const ApplicationSettings &settings = { };
    };

    class SIERRA_API Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        void Run();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::string_view GetName() const { return name; }
        [[nodiscard]] inline const Version& GetVersion() { return version; }

        /* --- OPERATORS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Application() = default;

    protected:
        explicit Application(const ApplicationCreateInfo &createInfo);

        [[nodiscard]] inline const FileManager& GetFileManager() const { return fileManager; }
        [[nodiscard]] inline const WindowManager& GetWindowManager() const { return *windowManager; }
        [[nodiscard]] inline const PlatformContext& GetPlatformContext() const { return *platformContext; }
        [[nodiscard]] inline const RenderingContext& GetRenderingContext() const { return *renderingContext; }

        [[nodiscard]] const std::filesystem::path& GetApplicationCachesDirectoryPath();
        [[nodiscard]] const std::filesystem::path& GetApplicationTemporaryDirectoryPath();

    private:
        virtual void Start() = 0;
        virtual bool Update(const TimeStep &timeStep) = 0;

        const std::string_view name;
        const Version version;
        const uint16 maxFrameRate;

        TimePoint lastFrameStartTime = TimePoint::Now();

        FileManager fileManager;
        std::unique_ptr<WindowManager> windowManager = nullptr;
        std::unique_ptr<PlatformContext> platformContext = nullptr;
        std::unique_ptr<RenderingContext> renderingContext = nullptr;

    };

}

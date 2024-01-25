//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Engine/Time.h"
#include "Engine/File.h"
#include "Core/Version.h"
#include "Core/WindowManager.h"
#include "Core/PlatformContext.h"
#include "Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        uint16 maxFrameRate = SR_PLATFORM_MOBILE * 60;
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    struct ApplicationCreateInfo
    {
        const std::string &name = "Sierra Application";
        const ApplicationSettings &settings = { };
        Version version = Version({ 1, 0, 0, VersionState::Experimental });
    };

    class SIERRA_API Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        void Run();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const std::string& GetName() const { return name; }
        [[nodiscard]] inline const Version& GetVersion() { return version; }

        /* --- OPERATORS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Application() = default;

    protected:
        explicit Application(const ApplicationCreateInfo &createInfo);

        [[nodiscard]] inline const PlatformContext& GetPlatformContext() { return *platformContext; }
        [[nodiscard]] inline const WindowManager& GetWindowManager() { return *windowManager; }
        [[nodiscard]] inline const RenderingContext& GetRenderingContext() { return *renderingContext; }

        [[nodiscard]] const std::filesystem::path& GetResourcesDirectoryPath();
        [[nodiscard]] const std::filesystem::path& GetApplicationCachesDirectoryPath();
        [[nodiscard]] const std::filesystem::path& GetApplicationTemporaryDirectoryPath();

    private:
        virtual void OnStart() = 0;
        virtual bool OnUpdate(const TimeStep &timeStep) = 0;

        std::string name;
        Version version;

        uint16 maxFrameRate = 0;
        TimePoint lastFrameStartTime = TimePoint::Now();

        std::unique_ptr<PlatformContext> platformContext = nullptr;
        std::unique_ptr<WindowManager> windowManager = nullptr;
        std::unique_ptr<RenderingContext> renderingContext = nullptr;
    };

}

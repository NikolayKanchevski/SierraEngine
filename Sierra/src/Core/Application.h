//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "FileManager.h"
#include "PlatformContext.h"
#include "Version.h"
#include "WindowManager.h"
#include "../Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    struct ApplicationCreateInfo
    {
        std::string_view name = "Sierra Application";
        Version version = Version({ 1, 0, 0 });
        const ApplicationSettings& settings = { };
    };

    class SIERRA_API Application
    {
    public:
        /* --- POLLING METHODS --- */
        virtual bool Update() = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const { return name; }
        [[nodiscard]] Version GetVersion() const { return version; }

        /* --- COPY SEMANTICS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /* --- MOVE SEMANTICS --- */
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Application() = default;

    protected:
        explicit Application(const ApplicationCreateInfo& createInfo);

        [[nodiscard]] const PlatformContext& GetPlatformContext() const { return *platformContext; }
        [[nodiscard]] const FileManager& GetFileManager() const { return *fileManager; }

        [[nodiscard]] const WindowManager& GetWindowManager() const { return *windowManager; }
        [[nodiscard]] const RenderingContext& GetRenderingContext() const { return *renderingContext; }

        [[nodiscard]] std::filesystem::path GetApplicationCachesDirectoryPath() const;
        [[nodiscard]] std::filesystem::path GetApplicationTemporaryDirectoryPath() const;

    private:
        const std::string_view name;
        const Version version;

        std::unique_ptr<PlatformContext> platformContext = nullptr;
        std::unique_ptr<FileManager> fileManager = nullptr;

        std::unique_ptr<WindowManager> windowManager = nullptr;
        std::unique_ptr<RenderingContext> renderingContext = nullptr;

    };

}

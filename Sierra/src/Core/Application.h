//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Version.h"
#include "../Files/FileManager.h"
#include "../Platform/PlatformContext.h"
#include "../Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        RenderingBackendType renderingBackendType = RenderingBackendType::Best;
    };

    struct ApplicationCreateInfo
    {
        std::string_view name = "Sierra Application";
        Version version = Version({ 1, 0, 0 });
        ApplicationSettings settings = { };
    };

    class SIERRA_API Application
    {
    public:
        /* --- POLLING METHODS --- */
        virtual bool Update();

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept { return name; }
        [[nodiscard]] Version GetVersion() const noexcept { return version; }

        /* --- COPY SEMANTICS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /* --- MOVE SEMANTICS --- */
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Application() = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Application(const ApplicationCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] const PlatformContext& GetPlatformContext() noexcept { return *platformContext; }
        [[nodiscard]] const RenderingContext& GetRenderingContext() const noexcept { return *renderingContext; }

        [[nodiscard]] const FileManager& GetFileManager() const noexcept { return platformContext->GetFileManager(); }

        [[nodiscard]] std::filesystem::path GetApplicationCachesDirectoryPath() const noexcept;
        [[nodiscard]] std::filesystem::path GetApplicationTemporaryDirectoryPath() const noexcept;

    private:
        const std::string_view name;
        const Version version;

        std::unique_ptr<PlatformContext> platformContext = nullptr;
        std::unique_ptr<RenderingContext> renderingContext = nullptr;

    };

}

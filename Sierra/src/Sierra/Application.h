//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Core/Version.h"
#include "Core/PlatformInstance.h"
#include "Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationCreateInfo
    {
        const String &name;
        Version version = Version({ 1, 0, 0, VersionState::Experimental });
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    class SIERRA_API Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        void Run();

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline String GetName() { return name; }
        [[nodiscard]] inline Version GetVersion() { return version; }

        [[nodiscard]] inline UniquePtr<PlatformInstance>& GetPlatformInstance() { return platformInstance; }
        [[nodiscard]] inline UniquePtr<RenderingContext>& GetRenderingContext() { return renderingContext; }

        /* --- DESTRUCTOR --- */
        virtual ~Application();

        /* --- OPERATORS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

    protected:
        explicit Application(const ApplicationCreateInfo &createInfo);

    private:
        /* --- POLLING METHODS --- */
        virtual void OnStart() = 0;
        virtual bool OnUpdate() = 0;

        String name;
        Version version;

        UniquePtr<PlatformInstance> platformInstance = nullptr;
        UniquePtr<RenderingContext> renderingContext = nullptr;

        void InitializeEngine();

    };

}

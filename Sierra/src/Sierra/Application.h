//
// Created by Nikolay Kanchevski on 21.08.23.
//

#pragma once

#include "Core/Version.h"
#include "Core/PlatformInstance.h"
#include "Rendering/RenderingContext.h"

namespace Sierra
{

    struct ApplicationSettings
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
        uint16 maxFrameRate = 0;
    };

    struct ApplicationCreateInfo
    {
        const String &name = "Sierra Application";
        const ApplicationSettings &settings = { };
        Version version = Version({ 1, 0, 0, VersionState::Experimental });
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
        virtual void OnStart() = 0;
        virtual bool OnUpdate(const TimeStep &timestep) = 0;

        String name;
        ApplicationSettings settings;
        Version version;

        UniquePtr<PlatformInstance> platformInstance = nullptr;
        UniquePtr<RenderingContext> renderingContext = nullptr;

        class FrameLimiter
        {
        public:
            /* --- CONSTRUCTORS --- */
            FrameLimiter() = default;

            /* --- POLLING METHODS --- */
            [[nodiscard]] TimeStep BeginFrame();
            void ThrottleFrame(uint32 targetFrameRate);

        private:
            TimePoint frameStartTime;
            TimePoint lastFrameStartTime;
            TimeStep deltaTime;

        };
        FrameLimiter frameLimiter{};


    };

}

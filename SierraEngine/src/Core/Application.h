//
// Created by Nikolay Kanchevski on 18.02.24.
//

#pragma once

namespace SierraEngine
{

    struct ApplicationCreateInfo
    {
        std::string_view name = "Sierra Application";
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
        Sierra::ApplicationSettings settings = { };
    };

    class SIERRA_ENGINE_API Application : public Sierra::Application
    {
    public:
        /* --- COPY SEMANTICS --- */
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        /* --- MOVE SEMANTICS --- */
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Application() override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit Application(const ApplicationCreateInfo& createInfo);

    };

}
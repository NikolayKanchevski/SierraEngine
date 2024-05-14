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
        const Sierra::ApplicationSettings &settings = { };
    };

    class SIERRA_ENGINE_API Application : public Sierra::Application
    {
    public:
        /* --- DESTRUCTOR --- */
        ~Application() override = default;

    protected:
        explicit Application(const ApplicationCreateInfo &createInfo);

    };

}
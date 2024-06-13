//
// Created by Nikolay Kanchevski on 13.05.24.
//

#pragma once

namespace SierraEngine
{

    class RuntimeApplication final : public Application
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit RuntimeApplication(const ApplicationCreateInfo &createInfo);

        /* --- DESTRUCTOR --- */
        ~RuntimeApplication() override = default;

    private:
        bool Update() override;

    };

}

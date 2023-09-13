//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

namespace Sierra
{

    BETTER_ENUM(
        PlatformType, uint8,
        Windows,
        Linux,
        MacOS
    );

    struct PlatformInstanceCreateInfo
    {

    };

    class PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        static UniquePtr<PlatformInstance> Create(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline virtual PlatformType GetType() const = 0;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformInstance() = default;

        /* --- OPERATORS --- */
        PlatformInstance(const PlatformInstance&) = delete;
        PlatformInstance& operator=(const PlatformInstance&) = delete;

    protected:
        explicit PlatformInstance(const PlatformInstanceCreateInfo &createInfo);

    };

}

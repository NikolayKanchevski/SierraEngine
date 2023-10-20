//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../Engine/Time.h"

namespace Sierra
{

    BETTER_ENUM(
        PlatformType, uint8,
        Windows,
        Linux,
        MacOS,
        iOS,
        Android
    );

    struct PlatformInstanceCreateInfo
    {

    };

    struct PlatformApplicationRunInfo
    {
        std::function<void()> OnStart;
        std::function<bool()> OnUpdate;
        std::function<void()> OnEnd;
    };

    class SIERRA_API PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        static UniquePtr<PlatformInstance> Create(const PlatformInstanceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        virtual void RunApplication(const PlatformApplicationRunInfo &runInfo);

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

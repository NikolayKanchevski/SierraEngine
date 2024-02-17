//
// Created by Nikolay Kanchevski on 30.11.23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the AndroidContext.h file is only allowed in Android builds!"
#endif

#include "../../PlatformContext.h"

#include "GameActivityContext.h"

namespace Sierra
{

    class SIERRA_API AndroidContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit AndroidContext(const PlatformContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const GameActivityContext& GetGameActivityContext() const { return activityContext; }
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::Android; }

    private:
        inline static android_app* app = nullptr;
        friend void ::android_main(android_app*);

        GameActivityContext activityContext;

    };

}
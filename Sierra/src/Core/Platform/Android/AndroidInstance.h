//
// Created by Nikolay Kanchevski on 30.11.23.
//

#pragma once

#include "../../PlatformInstance.h"

#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Sierra
{

    class SIERRA_API AndroidInstance final : public PlatformInstance
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit AndroidInstance(const PlatformInstanceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline PlatformType GetType() const override { return PlatformType::Android; }

    private:
        android_app* application = nullptr;
        void RunApplication(const PlatformApplicationRunInfo &runInfo) override;

    };

}
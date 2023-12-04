//
// Created by Nikolay Kanchevski on 1.12.23.
//

#pragma once

#include "../../src/srpch.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Sierra
{

    class SIERRA_API AndroidApplication
    {
    private:
        static inline android_app* application = nullptr;
        friend void ::android_main(android_app*);
        friend class AndroidInstance;

    };

}

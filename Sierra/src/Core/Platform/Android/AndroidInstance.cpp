//
// Created by Nikolay Kanchevski on 30.11.23.
//

#include "AndroidInstance.h"

#include "AndroidApplication.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    AndroidInstance::AndroidInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), application(AndroidApplication::application)
    {
//        android_app_set_key_event_filter(application, nullptr);
//        android_app_set_motion_event_filter(application, nullptr);
    }

    /* --- PRIVATE METHODS --- */

    void AndroidInstance::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        runInfo.OnStart();
        while (!runInfo.OnUpdate())
        {
//            // Poll events
//            int events;
//            android_poll_source* source;
//            while (ALooper_pollAll(-1, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0)
//            {
//                if (source != nullptr) source->process(application, source);
//                if (application->destroyRequested) break;
//            }
        }
        runInfo.OnEnd();
    }

}

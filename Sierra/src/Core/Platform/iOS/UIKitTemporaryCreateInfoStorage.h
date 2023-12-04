//
// Created by Nikolay Kanchevski on 10.11.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the UIKitTemporaryStorage.h file is only allowed in iOS builds!"
#endif

#include "UIKitContext.h"
#include "../../PlatformInstance.h" // For Sierra::PlatformApplicationRunInfo

namespace Sierra
{

    // We must store the data which the application will need to pick up globally, since data cannot in any way be passed directly to it,
    // as upon creation it will block the whole thread and only the run loop will continue to execute. Due to that, for each application,
    // we push its data to a queue, and, once it has been created we simply retrieve and move it out of the queue.

    struct UIKitApplicationCreateInfo
    {
        UIKitContext* context = nullptr;
        PlatformApplicationRunInfo runInfo = { };
    };

    class SIERRA_API UIKitTemporaryCreateInfoStorage
    {
    public:
        /* --- SETTER METHODS --- */
        static void Push(const UIKitApplicationCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        static UIKitApplicationCreateInfo MoveFront();

    private:
        static inline std::queue<UIKitApplicationCreateInfo> storageQueue;

    };

}

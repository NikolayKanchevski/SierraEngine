//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSContext.h"

#include "../../Windowing/iOS/UIKitWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSContext::iOSContext()
        : uiKitContext({ .application = [UIApplication sharedApplication] }), fileManager({ .fileManager = [NSFileManager defaultManager] })
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> iOSContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<UIKitWindow>(uiKitContext, createInfo);
    }

}

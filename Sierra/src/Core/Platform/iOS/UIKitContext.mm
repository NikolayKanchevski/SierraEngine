//
// Created by Nikolay Kanchevski on 9.11.23.
//

#include "UIKitContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitContext::UIKitContext(const Sierra::UIKitContextCreateInfo &createInfo)
        : application(createInfo.application), screen({ .screen = [UIScreen mainScreen] })
    {

    }

    /* --- POLLING METHODS --- */

    UIWindow* UIKitContext::CreateWindow() const
    {
        return [[UIWindow alloc] initWithWindowScene: reinterpret_cast<UIWindowScene*>([application connectedScenes].allObjects.firstObject)];
    }

    void UIKitContext::DestroyWindow(UIWindow* window) const
    {
        [window setRootViewController: nil];
        [window removeFromSuperview];
    }
}

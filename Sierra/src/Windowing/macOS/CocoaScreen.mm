//
// Created by Nikolay Kanchevski on 4.11.23.
//

#include "CocoaScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaScreen::CocoaScreen(const CocoaScreenCreateInfo& createInfo)
        : screen(createInfo.screen)
    {
        SR_THROW_IF(createInfo.screen == nil, InvalidValueError("Cannot create Cocoa screen, as specified screen must not be nil"));

        name = { [createInfo.screen localizedName].UTF8String, [createInfo.screen localizedName].length };
        refreshRate = { static_cast<uint16>(createInfo.screen.maximumFramesPerSecond) };

        origin = { createInfo.screen.frame.origin.x, createInfo.screen.frame.origin.y };
        size = { createInfo.screen.frame.size.width, createInfo.screen.frame.size.height };

        workAreaOrigin = { createInfo.screen.visibleFrame.origin.x, createInfo.screen.visibleFrame.origin.y };
        workAreaSize = { createInfo.screen.visibleFrame.size.width, createInfo.screen.visibleFrame.size.height };
    }

}
//
// Created by Nikolay Kanchevski on 4.11.23.
//

#include "CocoaScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaScreen::CocoaScreen(const CocoaScreenCreateInfo& createInfo)
        : Screen(), screen(createInfo.screen),
            name([createInfo.screen localizedName].UTF8String, [createInfo.screen localizedName].length),
            refreshRate(createInfo.screen.maximumFramesPerSecond),
            origin(createInfo.screen.frame.origin.x, createInfo.screen.frame.origin.y),
            size(createInfo.screen.frame.size.width, createInfo.screen.frame.size.height),
            workAreaOrigin(createInfo.screen.visibleFrame.origin.x, createInfo.screen.visibleFrame.origin.y),
            workAreaSize(createInfo.screen.visibleFrame.size.width, createInfo.screen.visibleFrame.size.height)
    {
        SR_ERROR_IF(createInfo.screen == nullptr, "NSScreen pointer passed upon creation of CocoaScreen must not be null!");
    }

}
//
// Created by Nikolay Kanchevski on 4.11.23.
//

#include "CocoaScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaScreen::CocoaScreen(const CocoaScreenCreateInfo &createInfo)
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

    /* --- MOVE SEMANTICS --- */

    CocoaScreen::CocoaScreen(CocoaScreen&& other)
        : screen(other.screen), name(std::move(other.name)), origin(other.origin), size(other.size), workAreaOrigin(other.workAreaOrigin), workAreaSize(other.workAreaSize), refreshRate(other.refreshRate)
    {
        other.screen = nil;
        other.name.clear();
        other.refreshRate = 0;

        other.origin = { 0, 0 };
        other.size = { 0, 0 };
        other.workAreaOrigin = { 0, 0 };
        other.workAreaSize = { 0, 0 };
    }

}
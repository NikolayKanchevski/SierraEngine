//
// Created by Nikolay Kanchevski on 4.11.23.
//

#include "CocoaScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaScreen::CocoaScreen(const CocoaScreenCreateInfo &createInfo)
        : Screen(), name(createInfo.nsScreen.localizedName.UTF8String),
          origin(createInfo.nsScreen.frame.origin.x, createInfo.nsScreen.frame.origin.y),
          size(createInfo.nsScreen.frame.size.width, createInfo.nsScreen.frame.size.height),
          workAreaOrigin(createInfo.nsScreen.visibleFrame.origin.x, createInfo.nsScreen.visibleFrame.origin.y),
          workAreaSize(createInfo.nsScreen.visibleFrame.size.width, createInfo.nsScreen.visibleFrame.size.height),
          refreshRate(createInfo.nsScreen.maximumFramesPerSecond)
    {

    }

    /* --- MOVE SEMANTICS --- */

    CocoaScreen::CocoaScreen(CocoaScreen&& other)
        : name(std::move(other.name)), origin(other.origin), size(other.size), workAreaOrigin(other.workAreaOrigin), workAreaSize(other.workAreaSize), refreshRate(other.refreshRate)
    {
        other.origin = { 0, 0 };
        other.size = { 0, 0 };
        other.workAreaOrigin = { 0, 0 };
        other.workAreaSize = { 0, 0 };
        other.refreshRate = 0;
    }

}
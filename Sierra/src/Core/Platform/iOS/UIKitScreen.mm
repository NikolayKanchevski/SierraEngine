//
// Created by Nikolay Kanchevski on 10.11.23.
//

#include "UIKitScreen.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitScreen::UIKitScreen(const UIKitScreenCreatInfo &createInfo)
        : Screen(), name([[UIDevice currentDevice] name].UTF8String),
          origin(createInfo.uiScreen.bounds.origin.x, createInfo.uiScreen.bounds.origin.y),
          size(createInfo.uiScreen.bounds.size.width, createInfo.uiScreen.bounds.size.height),
          refreshRate(static_cast<uint32>(createInfo.uiScreen.maximumFramesPerSecond))
    {
        // Create temporary window to get its extents
        const UIEdgeInsets safeAreaInsets = [UIApplication sharedApplication].windows.firstObject.safeAreaInsets;
        
        // Add up extents to get work area origin
        workAreaOrigin = origin;
        workAreaOrigin.x += static_cast<int32>(safeAreaInsets.left);
        workAreaOrigin.y += static_cast<int32>(safeAreaInsets.bottom);

        // Add up extents to get work area size
        workAreaSize = size;
        workAreaSize.x -= static_cast<uint32>(safeAreaInsets.left + safeAreaInsets.right);
        workAreaSize.y -= static_cast<uint32>(safeAreaInsets.top + safeAreaInsets.bottom);
    }

    /* --- GETTER METHODS --- */

    ScreenOrientation UIKitScreen::GetOrientation() const
    {
        switch ([[UIApplication sharedApplication].windows.firstObject.windowScene interfaceOrientation])
        {
            case UIInterfaceOrientationPortrait:           return ScreenOrientation::PortraitNormal;
            case UIInterfaceOrientationPortraitUpsideDown: return ScreenOrientation::PortraitFlipped;
            case UIInterfaceOrientationLandscapeLeft:      return ScreenOrientation::LandscapeNormal;
            case UIInterfaceOrientationLandscapeRight:     return ScreenOrientation::LandscapeFlipped;
            default:                                       return ScreenOrientation::Unknown;
        }

        return ScreenOrientation::Unknown;
    }
}

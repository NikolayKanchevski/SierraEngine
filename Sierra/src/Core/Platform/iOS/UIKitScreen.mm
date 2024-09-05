//
// Created by Nikolay Kanchevski on 10.11.23.
//

#include "UIKitScreen.h"

@interface UIKitScreenEventWrapper : NSObject

@end

@implementation UIKitScreenEventWrapper

    /* --- MEMBERS --- */
    {
        Sierra::UIKitScreen* screen;
    }

    /* --- CONSTRUCTORS --- */

    - (instancetype) initWithScreen: (Sierra::UIKitScreen*) initScreen
    {
        self = [super init];
        screen = initScreen;
        return self;
    }

    /* --- EVENTS --- */

    - (void) deviceOrientationDidChange: (NSNotification*) notification
    {
        Sierra::ScreenOrientation orientation;
        switch ([[UIApplication sharedApplication].windows.firstObject.windowScene interfaceOrientation])
        {
            case UIInterfaceOrientationPortrait:                { orientation = Sierra::ScreenOrientation::Portrait;         break; }
            case UIInterfaceOrientationPortraitUpsideDown:      { orientation = Sierra::ScreenOrientation::PortraitFlipped;  break; }
            case UIInterfaceOrientationLandscapeLeft:           { orientation = Sierra::ScreenOrientation::Landscape;        break; }
            case UIInterfaceOrientationLandscapeRight:          { orientation = Sierra::ScreenOrientation::LandscapeFlipped; break; }
            default:                                            { orientation = Sierra::ScreenOrientation::Unknown;          break; }
        }
        screen->RegisterScreenReorient(orientation);
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitScreen::UIKitScreen(const UIKitScreenCreatInfo& createInfo)
        : Screen(),
            eventWrapper([[UIKitScreenEventWrapper alloc] initWithScreen: this]),
            name([[UIDevice currentDevice].name UTF8String], [[UIDevice currentDevice].name length]),
            refreshRate(static_cast<uint32>(createInfo.screen.maximumFramesPerSecond)),
            orientation(GetOrientation()),
            origin(createInfo.screen.bounds.origin.x, createInfo.screen.bounds.origin.y),
            size(createInfo.screen.bounds.size.width, createInfo.screen.bounds.size.height)
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

        // Observe events
        [[NSNotificationCenter defaultCenter] addObserver: reinterpret_cast<UIKitScreenEventWrapper*>(eventWrapper) selector: @selector(deviceOrientationDidChange) name: UIDeviceOrientationDidChangeNotification object: nil];
    }

    /* --- POLLING METHODS --- */

    void UIKitScreen::RegisterScreenReorient(const ScreenOrientation reorientation)
    {
        if (
            orientation & ScreenOrientation::LandscapeAny && reorientation & ScreenOrientation::LandscapeAny ||
            orientation & ScreenOrientation::PortraitAny && reorientation & ScreenOrientation::PortraitAny
        )
        {
            return;
        }

        orientation = reorientation;
        GetScreenReorientDispatcher().DispatchEvent(reorientation);
    }
    
    /* --- DESTRUCTOR --- */

    UIKitScreen::~UIKitScreen()
    {
        [reinterpret_cast<UIKitScreenEventWrapper*>(eventWrapper) release];
    }

    /* --- CONVERSIONS --- */

    UIInterfaceOrientationMask UIKitScreen::ScreenOrientationToUIInterfaceOrientationMask(const ScreenOrientation orientation) 
    {
        switch (orientation)
        {
            case ScreenOrientation::Unknown:                return UIInterfaceOrientationUnknown;
            case ScreenOrientation::Portrait:               return UIInterfaceOrientationPortrait;
            case ScreenOrientation::PortraitFlipped:        return UIInterfaceOrientationPortraitUpsideDown;
            case ScreenOrientation::Landscape:              return UIInterfaceOrientationLandscapeLeft;
            case ScreenOrientation::LandscapeFlipped:       return UIInterfaceOrientationLandscapeRight;
            default:                                        break;
        }

        return UIInterfaceOrientationUnknown;
    }
    
}

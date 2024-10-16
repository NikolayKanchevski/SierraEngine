//
// Created by Nikolay Kanchevski on 10.11.23.
//

#include "UIKitScreen.h"

namespace
{
    Sierra::ScreenOrientation UIInterfaceOrientationToScreenOrientation(const UIInterfaceOrientation orientation)
    {
        switch (orientation)
        {
            case UIInterfaceOrientationPortrait:                return Sierra::ScreenOrientation::Portrait;
            case UIInterfaceOrientationPortraitUpsideDown:      return Sierra::ScreenOrientation::PortraitFlipped;
            case UIInterfaceOrientationLandscapeLeft:           return Sierra::ScreenOrientation::Landscape;
            case UIInterfaceOrientationLandscapeRight:          return Sierra::ScreenOrientation::LandscapeFlipped;
            default:                                            break;
        }

        return Sierra::ScreenOrientation::Unknown;;
    }
}

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
        const UIInterfaceOrientation orientation = [UIApplication sharedApplication].windows.firstObject.windowScene.interfaceOrientation;
        screen->RegisterScreenReorient(UIInterfaceOrientationToScreenOrientation(orientation));
    }

@end

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UIKitScreen::UIKitScreen(const UIKitScreenCreateInfo& createInfo)
        : eventWrapper([[UIKitScreenEventWrapper alloc] initWithScreen: this])
    {
        name = { [UIDevice currentDevice].name.UTF8String, [UIDevice currentDevice].name.length };
        refreshRate = static_cast<uint32>(createInfo.screen.maximumFramesPerSecond);

        orientation = UIInterfaceOrientationToScreenOrientation([UIApplication sharedApplication].windows.firstObject.windowScene.interfaceOrientation);

        origin = { static_cast<int32>(createInfo.screen.bounds.origin.x), static_cast<int32>(createInfo.screen.bounds.origin.y) };
        size = { static_cast<uint32>(createInfo.screen.bounds.size.width), static_cast<uint32>(createInfo.screen.bounds.size.height) };

        // Figure out window insets
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
        Screen::RegisterScreenReorient(reorientation);
        if (reorientation == orientation) return;

        orientation = reorientation;
        GetScreenReorientDispatcher().DispatchEvent(reorientation);
    }
    
    /* --- DESTRUCTOR --- */

    UIKitScreen::~UIKitScreen() noexcept
    {
        [reinterpret_cast<UIKitScreenEventWrapper*>(eventWrapper) release];
    }

}

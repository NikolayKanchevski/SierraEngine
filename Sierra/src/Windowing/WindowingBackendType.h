//
// Created by Nikolay Kanchevski on 8.10.24.
//

#pragma once

namespace Sierra
{

    enum class WindowingBackendType : uint8
    {
        Win32,
        X11,
        Cocoa,
        Activity,
        UIKit
    };

}
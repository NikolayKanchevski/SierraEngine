//
// Created by Nikolay Kanchevski on 10.28.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Extensions.h file is only allowed in Linux builds!"
#endif

#include "../../Windowing/Key.h"

#include <X11/Xlib.h>

namespace Sierra
{

    struct XkbExtensionCreateInfo
    {
        Display* display = nullptr;
    };

    class SIERRA_API XkbExtension final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit XkbExtension(const XkbExtensionCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsAutoRepeatDetectable() const { return autoRepeatDetectable; }
        [[nodiscard]] Key XKeyCodeToKey(const uint keyCode) const { return keyCode < keyTable.size() ? keyTable[keyCode] : Key::Unknown; }

    private:
        bool autoRepeatDetectable = false;
        std::array<Key, 256> keyTable = { };

        [[nodiscard]] Key TranslateKeySyms(const KeySym* keySyms, int32 width);

    };

    struct XrandrExtensionCreateInfo
    {
        Display* display = nullptr;
    };

    class SIERRA_API XrandrExtension final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit XrandrExtension(const XrandrExtensionCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] int GetEventBase() const { return eventBase; }

    private:
        int eventBase = 0;

    };

}

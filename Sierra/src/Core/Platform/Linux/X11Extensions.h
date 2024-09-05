//
// Created by Nikolay Kanchevski on 10.28.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Extensions.h file is only allowed in Linux builds!"
#endif

#include "../../Key.h"

#include <X11/Xlib.h>

namespace Sierra
{

    struct XkbExtensionCreateInfo
    {
        Display* display = nullptr;
    };

    class SIERRA_API XkbExtension final
    {
    private:
        constexpr static uint32 X11_KEY_ENTRY_COUNT = 256;

    public:
        /* --- CONSTRUCTORS --- */
        explicit XkbExtension(const XkbExtensionCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsAutoRepeatDetectable() const { return autoRepeatDetectable; }
        [[nodiscard]] const std::array<Key, X11_KEY_ENTRY_COUNT>& GetKeyTable() const { return KEY_TABLE; }

    private:
        bool autoRepeatDetectable = false;
        std::array<Key, X11_KEY_ENTRY_COUNT> KEY_TABLE { };
        static Key TranslateKeySyms(const KeySym* keySyms, int32 width);

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

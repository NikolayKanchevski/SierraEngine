//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

namespace Sierra
{

    enum class ScreenOrientation : uint8
    {
        Unknown                   = 0x0000,
        PortraitNormal            = 0x0001,
        PortraitFlipped           = 0x0002,
        Portrait                  = PortraitNormal | PortraitFlipped,
        LandscapeNormal           = 0x0004,
        LandscapeFlipped          = 0x0008,
        Landscape                 = LandscapeNormal | LandscapeFlipped
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(ScreenOrientation);

    class SIERRA_API Screen
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetName() const = 0;
        [[nodiscard]] virtual Vector2Int GetOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetSize() const = 0;
        [[nodiscard]] virtual Vector2Int GetWorkAreaOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetWorkAreaSize() const = 0;
        [[nodiscard]] virtual uint32 GetRefreshRate() const = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const = 0;

        /* --- OPERATORS --- */
        Screen(const Screen&) = delete;
        Screen& operator=(const Screen&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Screen() = default;

    protected:
        Screen() = default;

    };

}

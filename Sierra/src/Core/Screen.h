//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

namespace Sierra
{

    enum class ScreenOrientation : uint8
    {
        Unknown                   = 0 << 0,
        PortraitNormal            = 1 << 1,
        PortraitFlipped           = 1 << 2,
        Portrait                  = PortraitNormal | PortraitFlipped,
        LandscapeNormal           = 1 << 3,
        LandscapeFlipped          = 1 << 4,
        Landscape                 = LandscapeNormal | LandscapeFlipped
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(ScreenOrientation);

    class SIERRA_API Screen
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const std::string& GetName() const = 0;
        [[nodiscard]] virtual Vector2Int GetOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetSize() const = 0;
        [[nodiscard]] virtual Vector2Int GetWorkAreaOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetWorkAreaSize() const = 0;
        [[nodiscard]] virtual uint32 GetRefreshRate() const = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const = 0;

        /* --- DESTRUCTOR --- */
        virtual ~Screen() = default;

        /* --- OPERATORS --- */
        Screen(const Screen&) = delete;
        Screen& operator=(const Screen&) = delete;

    protected:
        Screen() = default;

    };

}

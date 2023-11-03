//
// Created by Nikolay Kanchevski on 10.31.23.
//

#pragma once

namespace Sierra
{

    class SIERRA_API Screen
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual String GetName() const = 0;
        [[nodiscard]] virtual Vector2Int GetOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetSize() const = 0;
        [[nodiscard]] virtual Vector2Int GetWorkAreaOrigin() const = 0;
        [[nodiscard]] virtual Vector2UInt GetWorkAreaSize() const = 0;
        [[nodiscard]] virtual int32 GetRefreshRate() const = 0;

        /* --- DESTRUCTOR --- */
        virtual ~Screen() = default;

        /* --- OPERATORS --- */
        Screen(const Screen&) = delete;
        Screen& operator=(const Screen&) = delete;

    protected:
        Screen() = default;

    };

}

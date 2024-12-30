//
// Created by Nikolay Kanchevski on 6.11.24.
//

#pragma once

namespace SierraEngine
{

    enum class EditorTheme : bool
    {
        Light,
        Dark
    };

    [[nodiscard]] ImGuiStyle EditorThemeToImGuiStyle(EditorTheme theme);

}
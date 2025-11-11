//
// Created by Nikolay Kanchevski on 4.11.25.
//

#pragma once

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using MenuItemCallback = std::function<void()>;

    struct MenuItem
    {
        std::string_view title = "Item";
        bool separator = false;

        bool enabled = true;
        const MenuItemCallback& Callback = nullptr;

        std::span<const MenuItem> items = { };
    };

    struct MenuPanelDrawInfo
    {
        std::span<const MenuItem> items = { };
    };

    namespace MenuPanel
    {
        void Draw(const MenuPanelDrawInfo& drawInfo);
    }

}

//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Tag.h"

#include "../../Editor/GUI.h"

namespace Sierra::Engine
{
    void Tag::OnDrawUI()
    {
        using namespace Editor;
        GUI::BeginProperties();

        GUI::StringProperty("Tag:", tag, "Some tooltip.");

        GUI::EndProperties();
    }
}

//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Tag.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine::Components
{
    void Tag::OnDrawUI()
    {
        GUI::BeginProperties();

        GUI::StringProperty("Tag:", tag, "Some tooltip.");

        GUI::EndProperties();
    }
}

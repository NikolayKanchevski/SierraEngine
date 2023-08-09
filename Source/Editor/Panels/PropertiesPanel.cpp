//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "PropertiesPanel.h"

#include "../GUI.h"
#include "../Editor.h"
#include "../../Engine/Components/Tag.h"
#include "../../Engine/Components/UUID.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/Components/Transform.h"
#include "../../Engine/Components/MeshRenderer.h"
#include "../../Engine/Components/PointLight.h"
#include "../../Engine/Components/DirectionalLight.h"

namespace Sierra::Editor
{
    using namespace Engine;

    /* --- POLLING METHODS --- */

    void PropertiesPanel::DrawUI()
    {
        if (GUI::BeginWindow("Properties", WindowFlags::NO_NAV))
        {
            if (!Editor::GetSelectedAsset().IsNull())
            {
                GUI::Text("Inspecting file...");
            }
            else if (Entity &entity = Editor::GetSelectedEntity(); !entity.IsNull())
            {
                GUI::DrawComponent<UUID>(entity);
                GUI::DrawComponent<Tag>(entity);
                GUI::DrawComponent<Transform>(entity);
                GUI::DrawComponent<MeshRenderer>(entity);
                GUI::DrawComponent<Camera>(entity);
                GUI::DrawComponent<DirectionalLight>(entity);
                GUI::DrawComponent<PointLight>(entity);
            }
        }
        GUI::EndWindow();
    }

}

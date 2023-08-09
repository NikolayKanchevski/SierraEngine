//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "PointLight.h"

#include "Transform.h"
#include "../../Editor/GUI.h"

namespace Sierra::Engine
{

    /* --- POLLING METHODS --- */

    void PointLight::OnDrawUI()
    {
        using namespace Editor;
        GUI::BeginProperties();

        GUI::FloatProperty("Intensity:", intensity);

        GUI::Vector3Property("Color:", color, "Some Tooltip");

        GUI::FloatProperty("Linear:", linear);
        GUI::FloatProperty("Quadratic:", quadratic);


        GUI::EndProperties();
    }

}
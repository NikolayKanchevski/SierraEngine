//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "PointLight.h"

#include "../Transform.h"
#include "../../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine
{
    /* --- CONSTRUCTORS --- */

    PointLight::PointLight()
    {
        lightID = IDPool.CreateNewID();
    }

    /* --- POLLING METHODS --- */

    void PointLight::OnDrawUI()
    {
        using namespace Rendering;
        GUI::BeginProperties();

        GUI::FloatProperty("Intensity:", intensity);

        static const float resetValues[3] = { 0.0f, 0.0f, 0.0f };
        static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
        GUI::PropertyVector3("Color:", color, resetValues, tooltips);

        GUI::FloatProperty("Linear:", linear);
        GUI::FloatProperty("Quadratic:", quadratic);


        GUI::EndProperties();
    }

    /* --- DESTRUCTORS --- */

    void PointLight::Destroy()
    {
        IDPool.RemoveID(lightID);
    }

    /* --- OPERATORS --- */

    PointLight::operator ShaderPointLight() const
    {
        auto position = GetComponent<Transform>().GetWorldPosition(); return
        {
            .projectionView = projectionView,
            .color = color,
            .intensity = intensity,
            .position = { position.x, -position.y, position.z },
            .linear = linear,
            .quadratic = quadratic
        };
    }
}
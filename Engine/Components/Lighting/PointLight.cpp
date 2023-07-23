//
// Created by Nikolay Kanchevski on 29.06.23.
//

#include "PointLight.h"

#include "../Transform.h"
#include "../../../Editor/GUI.h"

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
        using namespace Editor;
        GUI::BeginProperties();

        GUI::FloatProperty("Intensity:", intensity);

        GUI::Vector3Property("Color:", color, "Some Tooltip");

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
        return
        {
            .projectionView = projectionView,
            .color = color,
            .intensity = intensity,
            .position = GetComponent<Transform>().GetWorldPosition(),
            .linear = linear,
            .quadratic = quadratic
        };
    }
}
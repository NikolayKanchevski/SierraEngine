//
// Created by Nikolay Kanchevski on 2.07.23.
//

#include "Light.h"

#include "../Transform.h"

namespace Sierra::Engine
{
    void Light::OnAddComponent()
    {
        GetComponent<Transform>().PushOnDirtyCallback([this](const TransformDirtyFlag flag)
        {
            if (IS_FLAG_PRESENT(flag, TransformDirtyFlag::POSITION) || IS_FLAG_PRESENT(flag, TransformDirtyFlag::ROTATION)) Recalculate();
        });
    }

}
//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

#include "Components/Camera.h"
#include "Components/Tag.h"
#include "Components/Transform.h"
#include "Components/MeshRenderer.h"

namespace SierraEngine
{

    template<ComponentType... Components>
    struct ComponentGroup
    {
        template<ComponentType Component>
        constexpr static bool Contains = (std::is_same_v<Component, Components> || ...);
    };

    using AllComponents = ComponentGroup<Tag, Transform, Camera, MeshRenderer>;
    using RequiredComponents = ComponentGroup<Tag, Transform>;

}
//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include <Metal/Metal.hpp>
#include "../../RenderingResource.h"

namespace Sierra
{

    class SIERRA_API MetalResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline GraphicsAPI GetAPI() const override { return GraphicsAPI::Metal; };

    };

}

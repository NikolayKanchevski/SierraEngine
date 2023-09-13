//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    struct RenderingContextCreateInfo
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    class SIERRA_API RenderingContext : public virtual RenderingResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        static UniquePtr<RenderingContext> Create(const RenderingContextCreateInfo &createInfo);

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    protected:
        explicit RenderingContext(const RenderingContextCreateInfo &createInfo);

    };

}

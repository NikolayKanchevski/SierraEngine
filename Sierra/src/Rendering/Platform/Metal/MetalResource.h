//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../../RenderingResource.h"

#if defined(__OBJC__)
    #include <Metal/Metal.h>
    #include <QuartzCore/QuartzCore.h>
#endif

namespace Sierra
{

    class SIERRA_API MetalResource : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline GraphicsAPI GetAPI() const override { return GraphicsAPI::Metal; };

    protected:
        inline explicit MetalResource(const std::string &name)
        {
            #if SR_ENABLE_LOGGING
                this->name = name;
            #endif
        }

    };

}

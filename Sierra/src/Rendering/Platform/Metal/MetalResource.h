//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "../../RenderingResource.h"

#if SR_ENABLE_LOGGING
    #define MTL_SET_OBJECT_NAME(RESOURCE, NAME) const std::string stringName_##RESOURCE = NAME; RESOURCE->setLabel(NS::String::string(stringName_##RESOURCE.c_str(), NS::ASCIIStringEncoding))
#else
    #define MTL_SET_OBJECT_NAME(RESOURCE, NAME)
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

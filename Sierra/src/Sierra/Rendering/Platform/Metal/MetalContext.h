//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "MetalResource.h"
#include "../../RenderingContext.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalContext final : public RenderingContext, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalContext(const RenderingContextCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const MetalDevice& GetDevice() const override { return device; }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        MetalDevice device;

    };

}

//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#include "../../RenderPass.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalRenderPass : public RenderPass, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalRenderPass(const MetalDevice &device, const RenderPassCreateInfo &createInfo);

        /* --- DESTRUCTOR --- */
        void Destroy() override;

        /* --- CONVERSIONS --- */
        static MTL::LoadAction AttachmentLoadOperationToLoadAction(AttachmentLoadOperation loadOperation);
        static MTL::StoreAction AttachmentStoreOperationToStoreAction(AttachmentStoreOperation storeOperation);

    private:
        std::vector<MTL::RenderPassDescriptor*> renderPasses;

    };

}

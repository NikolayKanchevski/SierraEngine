//
// Created by Nikolay Kanchevski on 12.12.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateDevice()
    {
        // Create device
        device = Device::Create({ .requiredFeatures = {
            .sampleRateShading = msaaSamplingEnabled,
            .fillModeNonSolid = renderingMode != Fill,
            .samplerAnisotropy = VK_TRUE
        }});

        // Save device
        VulkanCore::SetDevice(device);

        // Lower MSAA if unsupported
        if (msaaSampleCount > device->GetHighestMultisampling())
        {
            ASSERT_WARNING("Requested MSAA of [" + std::to_string((int) msaaSampleCount) + "] but the highest supported is [" + std::to_string((int) device->GetHighestMultisampling()) + "]. The setting has been lowered automatically");
            msaaSampleCount = device->GetHighestMultisampling();
        }
    }

}
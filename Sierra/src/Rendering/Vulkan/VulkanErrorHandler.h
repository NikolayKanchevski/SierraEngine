//
// Created by Nikolay Kanchevski on 4.10.24.
//

#pragma once

namespace Sierra
{
    SIERRA_API void HandleVulkanError(VkResult result, std::string_view message);
}
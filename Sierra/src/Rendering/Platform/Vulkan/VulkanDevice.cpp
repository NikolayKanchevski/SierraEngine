//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include <vulkan/vulkan.h>
#include "VulkanDevice.h"

#if SR_PLATFORM_APPLE
    #include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION
    #define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR static_cast<VkStructureType>(1000163000)
#endif

#define VMA_IMPLEMENTATION
#ifdef VMA_STATS_STRING_ENABLED
    #undef VMA_STATS_STRING_ENABLED
    #define VMA_STATS_STRING_ENABLED 0
#endif
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanSwapchain.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDevice::VulkanDevice(const VulkanInstance &instance, const DeviceCreateInfo &createInfo)
        : Device(createInfo), VulkanResource(createInfo.name)
    {
        // Retrieve number of GPUs found
        uint32 physicalDeviceCount = 0;
        VkResult result = instance.GetFunctionTable().vkEnumeratePhysicalDevices(instance.GetVulkanInstance(), &physicalDeviceCount, nullptr);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not enumerate physical devices! Error code: {1}.", result);
        SR_ERROR_IF(physicalDeviceCount <= 0, "[Vulkan]: Could not find any supported physical devices!");

        // Retrieve GPUs
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        instance.GetFunctionTable().vkEnumeratePhysicalDevices(instance.GetVulkanInstance(), &physicalDeviceCount, physicalDevices.data());

        // Use first found device
        physicalDevice = physicalDevices[0];
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

        // Get count of all present queue families
        uint32 queueFamilyPropertiesCount = 0;
        instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);

        // Retrieve all queue families
        bool foundGeneralQueueFamily = false;
        std::vector<uint32> queueFamilies(queueFamilyPropertiesCount);
        if (queueFamilyPropertiesCount > 0)
        {
            // Retrieve queue families
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
            instance.GetFunctionTable().vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

            // Check which queues are present
            for (uint32 i = 0; i < queueFamilyPropertiesCount; i++)
            {
                const VkQueueFamilyProperties properties = queueFamilyProperties[i];

                // If queue family supports all operations, we save to use it later
                if (!foundGeneralQueueFamily && properties.queueFlags & VK_QUEUE_TRANSFER_BIT && properties.queueFlags & VK_QUEUE_COMPUTE_BIT && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    generalQueueFamily = i;
                    foundGeneralQueueFamily = true;
                }

                queueFamilies[i] = i;
            }
        }
        SR_ERROR_IF(!foundGeneralQueueFamily, "[Vulkan]: Could not create device [{0}], because it does not have a single queue family, which supports all operations!", GetName());

        // Set up queue create infos
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilies.size());

        uint32 i = 0;
        const float32 QUEUE_PRIORITY = 1.0f;
        for (const auto &queueFamily : queueFamilies)
        {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = queueFamily;
            queueCreateInfos[i].queueCount = 1;
            queueCreateInfos[i].pQueuePriorities = &QUEUE_PRIORITY;
            queueCreateInfos[i].flags = 0;
            queueCreateInfos[i].pNext = nullptr;
            i++;
        }

        // Store pointers of each extension's data, as it needs to be deallocated in the end of the function
        std::vector<void*> extensionDataToFree;
        extensionDataToFree.resize(DEVICE_EXTENSIONS_TO_QUERY.size());

        // Retrieve supported extension count
        uint32 supportedExtensionCount = 0;
        instance.GetFunctionTable().vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, nullptr);

        // Retrieve supported extensions
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        instance.GetFunctionTable().vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());

        // Set up device features
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 = { };
        physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

        // Load queried extensions if supported
        std::vector<const char*> extensionsToLoad;
        extensionsToLoad.reserve(DEVICE_EXTENSIONS_TO_QUERY.size() + SR_PLATFORM_APPLE); // Extra space for VK_KHR_PORTABILITY_SUBSET_EXTENSION (Apple-required-only extension)
        for (const auto &extension : DEVICE_EXTENSIONS_TO_QUERY)
        {
            AddExtensionIfSupported(extension, extensionsToLoad, supportedExtensions, physicalDeviceFeatures2, extensionDataToFree);
        }

        #if SR_PLATFORM_APPLE
            // Set up portability features
            VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubsetFeatures = { };
            if (IsExtensionSupported(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, supportedExtensions))
            {
                // Add extension to list
                extensionsToLoad.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);

                // Configure portability info
                portabilitySubsetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
                portabilitySubsetFeatures.pNext = nullptr;

                // Retrieve portability features
                PushToPNextChain(&physicalDeviceFeatures2, &portabilitySubsetFeatures);
                instance.GetFunctionTable().vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
            }
        #endif

        // Set to use already defined features
        physicalDeviceFeatures2.features = physicalDeviceFeatures;

        // Set up device create info
        VkDeviceCreateInfo logicalDeviceCreateInfo = { };
        logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
        logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        logicalDeviceCreateInfo.enabledLayerCount = 0;
        logicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;
        logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(extensionsToLoad.size());
        logicalDeviceCreateInfo.ppEnabledExtensionNames = extensionsToLoad.data();
        logicalDeviceCreateInfo.pEnabledFeatures = nullptr;
        logicalDeviceCreateInfo.flags = 0;
        logicalDeviceCreateInfo.pNext = &physicalDeviceFeatures2;

        // Create logical device
        result = instance.GetFunctionTable().vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create logical device [{0}]! Error code: {1}.", GetName(), result);

        // Deallocate extension data
        for (const auto &data : extensionDataToFree) std::free(data);

        // Load Vulkan functions
        #if defined(VK_VERSION_1_0)
            functionTable.vkAllocateCommandBuffers           = reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetDeviceProcAddr(logicalDevice, "vkAllocateCommandBuffers"));
            functionTable.vkAllocateDescriptorSets           = reinterpret_cast<PFN_vkAllocateDescriptorSets>(vkGetDeviceProcAddr(logicalDevice, "vkAllocateDescriptorSets"));
            functionTable.vkAllocateMemory                   = reinterpret_cast<PFN_vkAllocateMemory>(vkGetDeviceProcAddr(logicalDevice, "vkAllocateMemory"));
            functionTable.vkBeginCommandBuffer               = reinterpret_cast<PFN_vkBeginCommandBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkBeginCommandBuffer"));
            functionTable.vkBindBufferMemory                 = reinterpret_cast<PFN_vkBindBufferMemory>(vkGetDeviceProcAddr(logicalDevice, "vkBindBufferMemory"));
            functionTable.vkBindImageMemory                  = reinterpret_cast<PFN_vkBindImageMemory>(vkGetDeviceProcAddr(logicalDevice, "vkBindImageMemory"));
            functionTable.vkCmdBeginQuery                    = reinterpret_cast<PFN_vkCmdBeginQuery>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginQuery"));
            functionTable.vkCmdBeginRenderPass               = reinterpret_cast<PFN_vkCmdBeginRenderPass>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRenderPass"));
            functionTable.vkCmdBindDescriptorSets            = reinterpret_cast<PFN_vkCmdBindDescriptorSets>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindDescriptorSets"));
            functionTable.vkCmdBindIndexBuffer               = reinterpret_cast<PFN_vkCmdBindIndexBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindIndexBuffer"));
            functionTable.vkCmdBindPipeline                  = reinterpret_cast<PFN_vkCmdBindPipeline>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindPipeline"));
            functionTable.vkCmdBindVertexBuffers             = reinterpret_cast<PFN_vkCmdBindVertexBuffers>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindVertexBuffers"));
            functionTable.vkCmdBlitImage                     = reinterpret_cast<PFN_vkCmdBlitImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBlitImage"));
            functionTable.vkCmdClearAttachments              = reinterpret_cast<PFN_vkCmdClearAttachments>(vkGetDeviceProcAddr(logicalDevice, "vkCmdClearAttachments"));
            functionTable.vkCmdClearColorImage               = reinterpret_cast<PFN_vkCmdClearColorImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdClearColorImage"));
            functionTable.vkCmdClearDepthStencilImage        = reinterpret_cast<PFN_vkCmdClearDepthStencilImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdClearDepthStencilImage"));
            functionTable.vkCmdCopyBuffer                    = reinterpret_cast<PFN_vkCmdCopyBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBuffer"));
            functionTable.vkCmdCopyBufferToImage             = reinterpret_cast<PFN_vkCmdCopyBufferToImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBufferToImage"));
            functionTable.vkCmdCopyImage                     = reinterpret_cast<PFN_vkCmdCopyImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImage"));
            functionTable.vkCmdCopyImageToBuffer             = reinterpret_cast<PFN_vkCmdCopyImageToBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImageToBuffer"));
            functionTable.vkCmdCopyQueryPoolResults          = reinterpret_cast<PFN_vkCmdCopyQueryPoolResults>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyQueryPoolResults"));
            functionTable.vkCmdDispatch                      = reinterpret_cast<PFN_vkCmdDispatch>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDispatch"));
            functionTable.vkCmdDispatchIndirect              = reinterpret_cast<PFN_vkCmdDispatchIndirect>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDispatchIndirect"));
            functionTable.vkCmdDraw                          = reinterpret_cast<PFN_vkCmdDraw>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDraw"));
            functionTable.vkCmdDrawIndexed                   = reinterpret_cast<PFN_vkCmdDrawIndexed>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndexed"));
            functionTable.vkCmdDrawIndexedIndirect           = reinterpret_cast<PFN_vkCmdDrawIndexedIndirect>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndexedIndirect"));
            functionTable.vkCmdDrawIndirect                  = reinterpret_cast<PFN_vkCmdDrawIndirect>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndirect"));
            functionTable.vkCmdEndQuery                      = reinterpret_cast<PFN_vkCmdEndQuery>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndQuery"));
            functionTable.vkCmdEndRenderPass                 = reinterpret_cast<PFN_vkCmdEndRenderPass>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRenderPass"));
            functionTable.vkCmdExecuteCommands               = reinterpret_cast<PFN_vkCmdExecuteCommands>(vkGetDeviceProcAddr(logicalDevice, "vkCmdExecuteCommands"));
            functionTable.vkCmdFillBuffer                    = reinterpret_cast<PFN_vkCmdFillBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCmdFillBuffer"));
            functionTable.vkCmdNextSubpass                   = reinterpret_cast<PFN_vkCmdNextSubpass>(vkGetDeviceProcAddr(logicalDevice, "vkCmdNextSubpass"));
            functionTable.vkCmdPipelineBarrier               = reinterpret_cast<PFN_vkCmdPipelineBarrier>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPipelineBarrier"));
            functionTable.vkCmdPushConstants                 = reinterpret_cast<PFN_vkCmdPushConstants>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPushConstants"));
            functionTable.vkCmdResetEvent                    = reinterpret_cast<PFN_vkCmdResetEvent>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResetEvent"));
            functionTable.vkCmdResetQueryPool                = reinterpret_cast<PFN_vkCmdResetQueryPool>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResetQueryPool"));
            functionTable.vkCmdResolveImage                  = reinterpret_cast<PFN_vkCmdResolveImage>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResolveImage"));
            functionTable.vkCmdSetBlendConstants             = reinterpret_cast<PFN_vkCmdSetBlendConstants>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetBlendConstants"));
            functionTable.vkCmdSetDepthBias                  = reinterpret_cast<PFN_vkCmdSetDepthBias>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBias"));
            functionTable.vkCmdSetDepthBounds                = reinterpret_cast<PFN_vkCmdSetDepthBounds>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBounds"));
            functionTable.vkCmdSetEvent                      = reinterpret_cast<PFN_vkCmdSetEvent>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetEvent"));
            functionTable.vkCmdSetLineWidth                  = reinterpret_cast<PFN_vkCmdSetLineWidth>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLineWidth"));
            functionTable.vkCmdSetScissor                    = reinterpret_cast<PFN_vkCmdSetScissor>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetScissor"));
            functionTable.vkCmdSetStencilCompareMask         = reinterpret_cast<PFN_vkCmdSetStencilCompareMask>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilCompareMask"));
            functionTable.vkCmdSetStencilReference           = reinterpret_cast<PFN_vkCmdSetStencilReference>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilReference"));
            functionTable.vkCmdSetStencilWriteMask           = reinterpret_cast<PFN_vkCmdSetStencilWriteMask>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilWriteMask"));
            functionTable.vkCmdSetViewport                   = reinterpret_cast<PFN_vkCmdSetViewport>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewport"));
            functionTable.vkCmdUpdateBuffer                  = reinterpret_cast<PFN_vkCmdUpdateBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCmdUpdateBuffer"));
            functionTable.vkCmdWaitEvents                    = reinterpret_cast<PFN_vkCmdWaitEvents>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWaitEvents"));
            functionTable.vkCmdWriteTimestamp                = reinterpret_cast<PFN_vkCmdWriteTimestamp>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteTimestamp"));
            functionTable.vkCreateBuffer                     = reinterpret_cast<PFN_vkCreateBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCreateBuffer"));
            functionTable.vkCreateBufferView                 = reinterpret_cast<PFN_vkCreateBufferView>(vkGetDeviceProcAddr(logicalDevice, "vkCreateBufferView"));
            functionTable.vkCreateCommandPool                = reinterpret_cast<PFN_vkCreateCommandPool>(vkGetDeviceProcAddr(logicalDevice, "vkCreateCommandPool"));
            functionTable.vkCreateComputePipelines           = reinterpret_cast<PFN_vkCreateComputePipelines>(vkGetDeviceProcAddr(logicalDevice, "vkCreateComputePipelines"));
            functionTable.vkCreateDescriptorPool             = reinterpret_cast<PFN_vkCreateDescriptorPool>(vkGetDeviceProcAddr(logicalDevice, "vkCreateDescriptorPool"));
            functionTable.vkCreateDescriptorSetLayout        = reinterpret_cast<PFN_vkCreateDescriptorSetLayout>(vkGetDeviceProcAddr(logicalDevice, "vkCreateDescriptorSetLayout"));
            functionTable.vkCreateEvent                      = reinterpret_cast<PFN_vkCreateEvent>(vkGetDeviceProcAddr(logicalDevice, "vkCreateEvent"));
            functionTable.vkCreateFence                      = reinterpret_cast<PFN_vkCreateFence>(vkGetDeviceProcAddr(logicalDevice, "vkCreateFence"));
            functionTable.vkCreateFramebuffer                = reinterpret_cast<PFN_vkCreateFramebuffer>(vkGetDeviceProcAddr(logicalDevice, "vkCreateFramebuffer"));
            functionTable.vkCreateGraphicsPipelines          = reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetDeviceProcAddr(logicalDevice, "vkCreateGraphicsPipelines"));
            functionTable.vkCreateImage                      = reinterpret_cast<PFN_vkCreateImage>(vkGetDeviceProcAddr(logicalDevice, "vkCreateImage"));
            functionTable.vkCreateImageView                  = reinterpret_cast<PFN_vkCreateImageView>(vkGetDeviceProcAddr(logicalDevice, "vkCreateImageView"));
            functionTable.vkCreatePipelineCache              = reinterpret_cast<PFN_vkCreatePipelineCache>(vkGetDeviceProcAddr(logicalDevice, "vkCreatePipelineCache"));
            functionTable.vkCreatePipelineLayout             = reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetDeviceProcAddr(logicalDevice, "vkCreatePipelineLayout"));
            functionTable.vkCreateQueryPool                  = reinterpret_cast<PFN_vkCreateQueryPool>(vkGetDeviceProcAddr(logicalDevice, "vkCreateQueryPool"));
            functionTable.vkCreateRenderPass                 = reinterpret_cast<PFN_vkCreateRenderPass>(vkGetDeviceProcAddr(logicalDevice, "vkCreateRenderPass"));
            functionTable.vkCreateSampler                    = reinterpret_cast<PFN_vkCreateSampler>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSampler"));
            functionTable.vkCreateSemaphore                  = reinterpret_cast<PFN_vkCreateSemaphore>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSemaphore"));
            functionTable.vkCreateShaderModule               = reinterpret_cast<PFN_vkCreateShaderModule>(vkGetDeviceProcAddr(logicalDevice, "vkCreateShaderModule"));
            functionTable.vkDestroyBuffer                    = reinterpret_cast<PFN_vkDestroyBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyBuffer"));
            functionTable.vkDestroyBufferView                = reinterpret_cast<PFN_vkDestroyBufferView>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyBufferView"));
            functionTable.vkDestroyCommandPool               = reinterpret_cast<PFN_vkDestroyCommandPool>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyCommandPool"));
            functionTable.vkDestroyDescriptorPool            = reinterpret_cast<PFN_vkDestroyDescriptorPool>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDescriptorPool"));
            functionTable.vkDestroyDescriptorSetLayout       = reinterpret_cast<PFN_vkDestroyDescriptorSetLayout>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDescriptorSetLayout"));
            functionTable.vkDestroyDevice                    = reinterpret_cast<PFN_vkDestroyDevice>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDevice"));
            functionTable.vkDestroyEvent                     = reinterpret_cast<PFN_vkDestroyEvent>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyEvent"));
            functionTable.vkDestroyFence                     = reinterpret_cast<PFN_vkDestroyFence>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyFence"));
            functionTable.vkDestroyFramebuffer               = reinterpret_cast<PFN_vkDestroyFramebuffer>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyFramebuffer"));
            functionTable.vkDestroyImage                     = reinterpret_cast<PFN_vkDestroyImage>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyImage"));
            functionTable.vkDestroyImageView                 = reinterpret_cast<PFN_vkDestroyImageView>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyImageView"));
            functionTable.vkDestroyPipeline                  = reinterpret_cast<PFN_vkDestroyPipeline>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyPipeline"));
            functionTable.vkDestroyPipelineCache             = reinterpret_cast<PFN_vkDestroyPipelineCache>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyPipelineCache"));
            functionTable.vkDestroyPipelineLayout            = reinterpret_cast<PFN_vkDestroyPipelineLayout>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyPipelineLayout"));
            functionTable.vkDestroyQueryPool                 = reinterpret_cast<PFN_vkDestroyQueryPool>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyQueryPool"));
            functionTable.vkDestroyRenderPass                = reinterpret_cast<PFN_vkDestroyRenderPass>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyRenderPass"));
            functionTable.vkDestroySampler                   = reinterpret_cast<PFN_vkDestroySampler>(vkGetDeviceProcAddr(logicalDevice, "vkDestroySampler"));
            functionTable.vkDestroySemaphore                 = reinterpret_cast<PFN_vkDestroySemaphore>(vkGetDeviceProcAddr(logicalDevice, "vkDestroySemaphore"));
            functionTable.vkDestroyShaderModule              = reinterpret_cast<PFN_vkDestroyShaderModule>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyShaderModule"));
            functionTable.vkDeviceWaitIdle                   = reinterpret_cast<PFN_vkDeviceWaitIdle>(vkGetDeviceProcAddr(logicalDevice, "vkDeviceWaitIdle"));
            functionTable.vkEndCommandBuffer                 = reinterpret_cast<PFN_vkEndCommandBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkEndCommandBuffer"));
            functionTable.vkFlushMappedMemoryRanges          = reinterpret_cast<PFN_vkFlushMappedMemoryRanges>(vkGetDeviceProcAddr(logicalDevice, "vkFlushMappedMemoryRanges"));
            functionTable.vkFreeCommandBuffers               = reinterpret_cast<PFN_vkFreeCommandBuffers>(vkGetDeviceProcAddr(logicalDevice, "vkFreeCommandBuffers"));
            functionTable.vkFreeDescriptorSets               = reinterpret_cast<PFN_vkFreeDescriptorSets>(vkGetDeviceProcAddr(logicalDevice, "vkFreeDescriptorSets"));
            functionTable.vkFreeMemory                       = reinterpret_cast<PFN_vkFreeMemory>(vkGetDeviceProcAddr(logicalDevice, "vkFreeMemory"));
            functionTable.vkGetBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferMemoryRequirements"));
            functionTable.vkGetDeviceMemoryCommitment        = reinterpret_cast<PFN_vkGetDeviceMemoryCommitment>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceMemoryCommitment"));
            functionTable.vkGetDeviceQueue                   = reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceQueue"));
            functionTable.vkGetEventStatus                   = reinterpret_cast<PFN_vkGetEventStatus>(vkGetDeviceProcAddr(logicalDevice, "vkGetEventStatus"));
            functionTable.vkGetFenceStatus                   = reinterpret_cast<PFN_vkGetFenceStatus>(vkGetDeviceProcAddr(logicalDevice, "vkGetFenceStatus"));
            functionTable.vkGetImageMemoryRequirements       = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageMemoryRequirements"));
            functionTable.vkGetImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSparseMemoryRequirements"));
            functionTable.vkGetImageSubresourceLayout        = reinterpret_cast<PFN_vkGetImageSubresourceLayout>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSubresourceLayout"));
            functionTable.vkGetPipelineCacheData             = reinterpret_cast<PFN_vkGetPipelineCacheData>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineCacheData"));
            functionTable.vkGetQueryPoolResults              = reinterpret_cast<PFN_vkGetQueryPoolResults>(vkGetDeviceProcAddr(logicalDevice, "vkGetQueryPoolResults"));
            functionTable.vkGetRenderAreaGranularity         = reinterpret_cast<PFN_vkGetRenderAreaGranularity>(vkGetDeviceProcAddr(logicalDevice, "vkGetRenderAreaGranularity"));
            functionTable.vkInvalidateMappedMemoryRanges     = reinterpret_cast<PFN_vkInvalidateMappedMemoryRanges>(vkGetDeviceProcAddr(logicalDevice, "vkInvalidateMappedMemoryRanges"));
            functionTable.vkMapMemory                        = reinterpret_cast<PFN_vkMapMemory>(vkGetDeviceProcAddr(logicalDevice, "vkMapMemory"));
            functionTable.vkMergePipelineCaches              = reinterpret_cast<PFN_vkMergePipelineCaches>(vkGetDeviceProcAddr(logicalDevice, "vkMergePipelineCaches"));
            functionTable.vkQueueBindSparse                  = reinterpret_cast<PFN_vkQueueBindSparse>(vkGetDeviceProcAddr(logicalDevice, "vkQueueBindSparse"));
            functionTable.vkQueueSubmit                      = reinterpret_cast<PFN_vkQueueSubmit>(vkGetDeviceProcAddr(logicalDevice, "vkQueueSubmit"));
            functionTable.vkQueueWaitIdle                    = reinterpret_cast<PFN_vkQueueWaitIdle>(vkGetDeviceProcAddr(logicalDevice, "vkQueueWaitIdle"));
            functionTable.vkResetCommandBuffer               = reinterpret_cast<PFN_vkResetCommandBuffer>(vkGetDeviceProcAddr(logicalDevice, "vkResetCommandBuffer"));
            functionTable.vkResetCommandPool                 = reinterpret_cast<PFN_vkResetCommandPool>(vkGetDeviceProcAddr(logicalDevice, "vkResetCommandPool"));
            functionTable.vkResetDescriptorPool              = reinterpret_cast<PFN_vkResetDescriptorPool>(vkGetDeviceProcAddr(logicalDevice, "vkResetDescriptorPool"));
            functionTable.vkResetEvent                       = reinterpret_cast<PFN_vkResetEvent>(vkGetDeviceProcAddr(logicalDevice, "vkResetEvent"));
            functionTable.vkResetFences                      = reinterpret_cast<PFN_vkResetFences>(vkGetDeviceProcAddr(logicalDevice, "vkResetFences"));
            functionTable.vkSetEvent                         = reinterpret_cast<PFN_vkSetEvent>(vkGetDeviceProcAddr(logicalDevice, "vkSetEvent"));
            functionTable.vkUnmapMemory                      = reinterpret_cast<PFN_vkUnmapMemory>(vkGetDeviceProcAddr(logicalDevice, "vkUnmapMemory"));
            functionTable.vkUpdateDescriptorSets             = reinterpret_cast<PFN_vkUpdateDescriptorSets>(vkGetDeviceProcAddr(logicalDevice, "vkUpdateDescriptorSets"));
            functionTable.vkWaitForFences                    = reinterpret_cast<PFN_vkWaitForFences>(vkGetDeviceProcAddr(logicalDevice, "vkWaitForFences"));
        #endif
        #if defined(VK_VERSION_1_1)
            functionTable.vkBindBufferMemory2                 = reinterpret_cast<PFN_vkBindBufferMemory2>(vkGetDeviceProcAddr(logicalDevice, "vkBindBufferMemory2"));
            functionTable.vkBindImageMemory2                  = reinterpret_cast<PFN_vkBindImageMemory2>(vkGetDeviceProcAddr(logicalDevice, "vkBindImageMemory2"));
            functionTable.vkCmdDispatchBase                   = reinterpret_cast<PFN_vkCmdDispatchBase>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDispatchBase"));
            functionTable.vkCmdSetDeviceMask                  = reinterpret_cast<PFN_vkCmdSetDeviceMask>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDeviceMask"));
            functionTable.vkCreateDescriptorUpdateTemplate    = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplate>(vkGetDeviceProcAddr(logicalDevice, "vkCreateDescriptorUpdateTemplate"));
            functionTable.vkCreateSamplerYcbcrConversion      = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversion>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSamplerYcbcrConversion"));
            functionTable.vkDestroyDescriptorUpdateTemplate   = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplate>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDescriptorUpdateTemplate"));
            functionTable.vkDestroySamplerYcbcrConversion     = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversion>(vkGetDeviceProcAddr(logicalDevice, "vkDestroySamplerYcbcrConversion"));
            functionTable.vkGetBufferMemoryRequirements2      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferMemoryRequirements2"));
            functionTable.vkGetDescriptorSetLayoutSupport     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupport>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetLayoutSupport"));
            functionTable.vkGetDeviceGroupPeerMemoryFeatures  = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeatures>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceGroupPeerMemoryFeatures"));
            functionTable.vkGetDeviceQueue2                   = reinterpret_cast<PFN_vkGetDeviceQueue2>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceQueue2"));
            functionTable.vkGetImageMemoryRequirements2       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageMemoryRequirements2"));
            functionTable.vkGetImageSparseMemoryRequirements2 = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSparseMemoryRequirements2"));
            functionTable.vkTrimCommandPool                   = reinterpret_cast<PFN_vkTrimCommandPool>(vkGetDeviceProcAddr(logicalDevice, "vkTrimCommandPool"));
            functionTable.vkUpdateDescriptorSetWithTemplate   = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplate>(vkGetDeviceProcAddr(logicalDevice, "vkUpdateDescriptorSetWithTemplate"));
        #endif
        #if defined(VK_VERSION_1_2)
            functionTable.vkCmdBeginRenderPass2                 = reinterpret_cast<PFN_vkCmdBeginRenderPass2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRenderPass2"));
            functionTable.vkCmdDrawIndexedIndirectCount         = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCount>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndexedIndirectCount"));
            functionTable.vkCmdDrawIndirectCount                = reinterpret_cast<PFN_vkCmdDrawIndirectCount>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndirectCount"));
            functionTable.vkCmdEndRenderPass2                   = reinterpret_cast<PFN_vkCmdEndRenderPass2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRenderPass2"));
            functionTable.vkCmdNextSubpass2                     = reinterpret_cast<PFN_vkCmdNextSubpass2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdNextSubpass2"));
            functionTable.vkCreateRenderPass2                   = reinterpret_cast<PFN_vkCreateRenderPass2>(vkGetDeviceProcAddr(logicalDevice, "vkCreateRenderPass2"));
            functionTable.vkGetBufferDeviceAddress              = reinterpret_cast<PFN_vkGetBufferDeviceAddress>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferDeviceAddress"));
            functionTable.vkGetBufferOpaqueCaptureAddress       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddress>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferOpaqueCaptureAddress"));
            functionTable.vkGetDeviceMemoryOpaqueCaptureAddress = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddress>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceMemoryOpaqueCaptureAddress"));
            functionTable.vkGetSemaphoreCounterValue            = reinterpret_cast<PFN_vkGetSemaphoreCounterValue>(vkGetDeviceProcAddr(logicalDevice, "vkGetSemaphoreCounterValue"));
            functionTable.vkResetQueryPool                      = reinterpret_cast<PFN_vkResetQueryPool>(vkGetDeviceProcAddr(logicalDevice, "vkResetQueryPool"));
            functionTable.vkSignalSemaphore                     = reinterpret_cast<PFN_vkSignalSemaphore>(vkGetDeviceProcAddr(logicalDevice, "vkSignalSemaphore"));
            functionTable.vkWaitSemaphores                      = reinterpret_cast<PFN_vkWaitSemaphores>(vkGetDeviceProcAddr(logicalDevice, "vkWaitSemaphores"));
        #endif
        #if defined(VK_VERSION_1_3)
            functionTable.vkCmdBeginRendering                      = reinterpret_cast<PFN_vkCmdBeginRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRendering"));
            functionTable.vkCmdBindVertexBuffers2                  = reinterpret_cast<PFN_vkCmdBindVertexBuffers2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindVertexBuffers2"));
            functionTable.vkCmdBlitImage2                          = reinterpret_cast<PFN_vkCmdBlitImage2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBlitImage2"));
            functionTable.vkCmdCopyBuffer2                         = reinterpret_cast<PFN_vkCmdCopyBuffer2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBuffer2"));
            functionTable.vkCmdCopyBufferToImage2                  = reinterpret_cast<PFN_vkCmdCopyBufferToImage2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBufferToImage2"));
            functionTable.vkCmdCopyImage2                          = reinterpret_cast<PFN_vkCmdCopyImage2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImage2"));
            functionTable.vkCmdCopyImageToBuffer2                  = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImageToBuffer2"));
            functionTable.vkCmdEndRendering                        = reinterpret_cast<PFN_vkCmdEndRendering>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRendering"));
            functionTable.vkCmdPipelineBarrier2                    = reinterpret_cast<PFN_vkCmdPipelineBarrier2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPipelineBarrier2"));
            functionTable.vkCmdResetEvent2                         = reinterpret_cast<PFN_vkCmdResetEvent2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResetEvent2"));
            functionTable.vkCmdResolveImage2                       = reinterpret_cast<PFN_vkCmdResolveImage2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResolveImage2"));
            functionTable.vkCmdSetCullMode                         = reinterpret_cast<PFN_vkCmdSetCullMode>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCullMode"));
            functionTable.vkCmdSetDepthBiasEnable                  = reinterpret_cast<PFN_vkCmdSetDepthBiasEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBiasEnable"));
            functionTable.vkCmdSetDepthBoundsTestEnable            = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBoundsTestEnable"));
            functionTable.vkCmdSetDepthCompareOp                   = reinterpret_cast<PFN_vkCmdSetDepthCompareOp>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthCompareOp"));
            functionTable.vkCmdSetDepthTestEnable                  = reinterpret_cast<PFN_vkCmdSetDepthTestEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthTestEnable"));
            functionTable.vkCmdSetDepthWriteEnable                 = reinterpret_cast<PFN_vkCmdSetDepthWriteEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthWriteEnable"));
            functionTable.vkCmdSetEvent2                           = reinterpret_cast<PFN_vkCmdSetEvent2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetEvent2"));
            functionTable.vkCmdSetFrontFace                        = reinterpret_cast<PFN_vkCmdSetFrontFace>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetFrontFace"));
            functionTable.vkCmdSetPrimitiveRestartEnable           = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPrimitiveRestartEnable"));
            functionTable.vkCmdSetPrimitiveTopology                = reinterpret_cast<PFN_vkCmdSetPrimitiveTopology>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPrimitiveTopology"));
            functionTable.vkCmdSetRasterizerDiscardEnable          = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRasterizerDiscardEnable"));
            functionTable.vkCmdSetScissorWithCount                 = reinterpret_cast<PFN_vkCmdSetScissorWithCount>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetScissorWithCount"));
            functionTable.vkCmdSetStencilOp                        = reinterpret_cast<PFN_vkCmdSetStencilOp>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilOp"));
            functionTable.vkCmdSetStencilTestEnable                = reinterpret_cast<PFN_vkCmdSetStencilTestEnable>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilTestEnable"));
            functionTable.vkCmdSetViewportWithCount                = reinterpret_cast<PFN_vkCmdSetViewportWithCount>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportWithCount"));
            functionTable.vkCmdWaitEvents2                         = reinterpret_cast<PFN_vkCmdWaitEvents2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWaitEvents2"));
            functionTable.vkCmdWriteTimestamp2                     = reinterpret_cast<PFN_vkCmdWriteTimestamp2>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteTimestamp2"));
            functionTable.vkCreatePrivateDataSlot                  = reinterpret_cast<PFN_vkCreatePrivateDataSlot>(vkGetDeviceProcAddr(logicalDevice, "vkCreatePrivateDataSlot"));
            functionTable.vkDestroyPrivateDataSlot                 = reinterpret_cast<PFN_vkDestroyPrivateDataSlot>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyPrivateDataSlot"));
            functionTable.vkGetDeviceBufferMemoryRequirements      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceBufferMemoryRequirements"));
            functionTable.vkGetDeviceImageMemoryRequirements       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceImageMemoryRequirements"));
            functionTable.vkGetDeviceImageSparseMemoryRequirements = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirements>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceImageSparseMemoryRequirements"));
            functionTable.vkGetPrivateData                         = reinterpret_cast<PFN_vkGetPrivateData>(vkGetDeviceProcAddr(logicalDevice, "vkGetPrivateData"));
            functionTable.vkQueueSubmit2                           = reinterpret_cast<PFN_vkQueueSubmit2>(vkGetDeviceProcAddr(logicalDevice, "vkQueueSubmit2"));
            functionTable.vkSetPrivateData                         = reinterpret_cast<PFN_vkSetPrivateData>(vkGetDeviceProcAddr(logicalDevice, "vkSetPrivateData"));
        #endif
        #if defined(VK_AMD_buffer_marker)
            functionTable.vkCmdWriteBufferMarkerAMD = reinterpret_cast<PFN_vkCmdWriteBufferMarkerAMD>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteBufferMarkerAMD"));
        #endif
        #if defined(VK_AMD_display_native_hdr)
            functionTable.vkSetLocalDimmingAMD = reinterpret_cast<PFN_vkSetLocalDimmingAMD>(vkGetDeviceProcAddr(logicalDevice, "vkSetLocalDimmingAMD"));
        #endif
        #if defined(VK_AMD_draw_indirect_count)
            functionTable.vkCmdDrawIndexedIndirectCountAMD = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountAMD>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndexedIndirectCountAMD"));
            functionTable.vkCmdDrawIndirectCountAMD        = reinterpret_cast<PFN_vkCmdDrawIndirectCountAMD>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndirectCountAMD"));
        #endif
        #if defined(VK_AMD_shader_info)
            functionTable.vkGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>(vkGetDeviceProcAddr(logicalDevice, "vkGetShaderInfoAMD"));
        #endif
        #if defined(VK_ANDROID_external_memory_android_hardware_buffer)
            functionTable.vkGetAndroidHardwareBufferPropertiesANDROID = reinterpret_cast<PFN_vkGetAndroidHardwareBufferPropertiesANDROID>(vkGetDeviceProcAddr(logicalDevice, "vkGetAndroidHardwareBufferPropertiesANDROID"));
            functionTable.vkGetMemoryAndroidHardwareBufferANDROID     = reinterpret_cast<PFN_vkGetMemoryAndroidHardwareBufferANDROID>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryAndroidHardwareBufferANDROID"));
        #endif
        #if defined(VK_EXT_attachment_feedback_loop_dynamic_state)
            functionTable.vkCmdSetAttachmentFeedbackLoopEnableEXT = reinterpret_cast<PFN_vkCmdSetAttachmentFeedbackLoopEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetAttachmentFeedbackLoopEnableEXT"));
        #endif
        #if defined(VK_EXT_buffer_device_address)
            functionTable.vkGetBufferDeviceAddressEXT = reinterpret_cast<PFN_vkGetBufferDeviceAddressEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferDeviceAddressEXT"));
        #endif
        #if defined(VK_EXT_calibrated_timestamps)
            functionTable.vkGetCalibratedTimestampsEXT = reinterpret_cast<PFN_vkGetCalibratedTimestampsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetCalibratedTimestampsEXT"));
        #endif
        #if defined(VK_EXT_color_write_enable)
            functionTable.vkCmdSetColorWriteEnableEXT = reinterpret_cast<PFN_vkCmdSetColorWriteEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetColorWriteEnableEXT"));
        #endif
        #if defined(VK_EXT_conditional_rendering)
            functionTable.vkCmdBeginConditionalRenderingEXT = reinterpret_cast<PFN_vkCmdBeginConditionalRenderingEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginConditionalRenderingEXT"));
            functionTable.vkCmdEndConditionalRenderingEXT   = reinterpret_cast<PFN_vkCmdEndConditionalRenderingEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndConditionalRenderingEXT"));
        #endif
        #if defined(VK_EXT_debug_marker)
            functionTable.vkCmdDebugMarkerBeginEXT      = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerBeginEXT"));
            functionTable.vkCmdDebugMarkerEndEXT        = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerEndEXT"));
            functionTable.vkCmdDebugMarkerInsertEXT     = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerInsertEXT"));
            functionTable.vkDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectNameEXT"));
            functionTable.vkDebugMarkerSetObjectTagEXT  = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectTagEXT"));
        #endif
        #if defined(VK_EXT_depth_bias_control)
            functionTable.vkCmdSetDepthBias2EXT = reinterpret_cast<PFN_vkCmdSetDepthBias2EXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBias2EXT"));
        #endif
        #if defined(VK_EXT_descriptor_buffer)
            functionTable.vkCmdBindDescriptorBufferEmbeddedSamplersEXT = reinterpret_cast<PFN_vkCmdBindDescriptorBufferEmbeddedSamplersEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindDescriptorBufferEmbeddedSamplersEXT"));
            functionTable.vkCmdBindDescriptorBuffersEXT                = reinterpret_cast<PFN_vkCmdBindDescriptorBuffersEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindDescriptorBuffersEXT"));
            functionTable.vkCmdSetDescriptorBufferOffsetsEXT           = reinterpret_cast<PFN_vkCmdSetDescriptorBufferOffsetsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDescriptorBufferOffsetsEXT"));
            functionTable.vkGetBufferOpaqueCaptureDescriptorDataEXT    = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferOpaqueCaptureDescriptorDataEXT"));
            functionTable.vkGetDescriptorEXT                           = reinterpret_cast<PFN_vkGetDescriptorEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorEXT"));
            functionTable.vkGetDescriptorSetLayoutBindingOffsetEXT     = reinterpret_cast<PFN_vkGetDescriptorSetLayoutBindingOffsetEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetLayoutBindingOffsetEXT"));
            functionTable.vkGetDescriptorSetLayoutSizeEXT              = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSizeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetLayoutSizeEXT"));
            functionTable.vkGetImageOpaqueCaptureDescriptorDataEXT     = reinterpret_cast<PFN_vkGetImageOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageOpaqueCaptureDescriptorDataEXT"));
            functionTable.vkGetImageViewOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetImageViewOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageViewOpaqueCaptureDescriptorDataEXT"));
            functionTable.vkGetSamplerOpaqueCaptureDescriptorDataEXT   = reinterpret_cast<PFN_vkGetSamplerOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetSamplerOpaqueCaptureDescriptorDataEXT"));
        #endif
        #if defined(VK_EXT_descriptor_buffer) && (defined(VK_KHR_acceleration_structure) || defined(VK_NV_ray_tracing))
            functionTable.vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT = reinterpret_cast<PFN_vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureOpaqueCaptureDescriptorDataEXT"));
        #endif
        #if defined(VK_EXT_device_fault)
            functionTable.vkGetDeviceFaultInfoEXT = reinterpret_cast<PFN_vkGetDeviceFaultInfoEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceFaultInfoEXT"));
        #endif
        #if defined(VK_EXT_discard_rectangles)
            functionTable.vkCmdSetDiscardRectangleEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDiscardRectangleEXT"));
        #endif
        #if defined(VK_EXT_discard_rectangles) && VK_EXT_DISCARD_RECTANGLES_SPEC_VERSION >= 2
            functionTable.vkCmdSetDiscardRectangleEnableEXT = reinterpret_cast<PFN_vkCmdSetDiscardRectangleEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDiscardRectangleEnableEXT"));
            functionTable.vkCmdSetDiscardRectangleModeEXT   = reinterpret_cast<PFN_vkCmdSetDiscardRectangleModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDiscardRectangleModeEXT"));
        #endif
        #if defined(VK_EXT_display_control)
            functionTable.vkDisplayPowerControlEXT  = reinterpret_cast<PFN_vkDisplayPowerControlEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDisplayPowerControlEXT"));
            functionTable.vkGetSwapchainCounterEXT  = reinterpret_cast<PFN_vkGetSwapchainCounterEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetSwapchainCounterEXT"));
            functionTable.vkRegisterDeviceEventEXT  = reinterpret_cast<PFN_vkRegisterDeviceEventEXT>(vkGetDeviceProcAddr(logicalDevice, "vkRegisterDeviceEventEXT"));
            functionTable.vkRegisterDisplayEventEXT = reinterpret_cast<PFN_vkRegisterDisplayEventEXT>(vkGetDeviceProcAddr(logicalDevice, "vkRegisterDisplayEventEXT"));
        #endif
        #if defined(VK_EXT_external_memory_host)
            functionTable.vkGetMemoryHostPointerPropertiesEXT = reinterpret_cast<PFN_vkGetMemoryHostPointerPropertiesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryHostPointerPropertiesEXT"));
        #endif
        #if defined(VK_EXT_full_screen_exclusive)
            functionTable.vkAcquireFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkAcquireFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkAcquireFullScreenExclusiveModeEXT"));
            functionTable.vkReleaseFullScreenExclusiveModeEXT = reinterpret_cast<PFN_vkReleaseFullScreenExclusiveModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkReleaseFullScreenExclusiveModeEXT"));
        #endif
        #if defined(VK_EXT_hdr_metadata)
            functionTable.vkSetHdrMetadataEXT = reinterpret_cast<PFN_vkSetHdrMetadataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkSetHdrMetadataEXT"));
        #endif
        #if defined(VK_EXT_host_image_copy)
            functionTable.vkCopyImageToImageEXT      = reinterpret_cast<PFN_vkCopyImageToImageEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyImageToImageEXT"));
            functionTable.vkCopyImageToMemoryEXT     = reinterpret_cast<PFN_vkCopyImageToMemoryEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyImageToMemoryEXT"));
            functionTable.vkCopyMemoryToImageEXT     = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyMemoryToImageEXT"));
            functionTable.vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetDeviceProcAddr(logicalDevice, "vkTransitionImageLayoutEXT"));
        #endif
        #if defined(VK_EXT_host_query_reset)
            functionTable.vkResetQueryPoolEXT = reinterpret_cast<PFN_vkResetQueryPoolEXT>(vkGetDeviceProcAddr(logicalDevice, "vkResetQueryPoolEXT"));
        #endif
        #if defined(VK_EXT_image_drm_format_modifier)
            functionTable.vkGetImageDrmFormatModifierPropertiesEXT = reinterpret_cast<PFN_vkGetImageDrmFormatModifierPropertiesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageDrmFormatModifierPropertiesEXT"));
        #endif
        #if defined(VK_EXT_line_rasterization)
            functionTable.vkCmdSetLineStippleEXT = reinterpret_cast<PFN_vkCmdSetLineStippleEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLineStippleEXT"));
        #endif
        #if defined(VK_EXT_mesh_shader)
            functionTable.vkCmdDrawMeshTasksEXT              = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksEXT"));
            functionTable.vkCmdDrawMeshTasksIndirectCountEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksIndirectCountEXT"));
            functionTable.vkCmdDrawMeshTasksIndirectEXT      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksIndirectEXT"));
        #endif
        #if defined(VK_EXT_metal_objects)
            functionTable.vkExportMetalObjectsEXT = reinterpret_cast<PFN_vkExportMetalObjectsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkExportMetalObjectsEXT"));
        #endif
        #if defined(VK_EXT_multi_draw)
            functionTable.vkCmdDrawMultiEXT        = reinterpret_cast<PFN_vkCmdDrawMultiEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMultiEXT"));
            functionTable.vkCmdDrawMultiIndexedEXT = reinterpret_cast<PFN_vkCmdDrawMultiIndexedEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMultiIndexedEXT"));
        #endif
        #if defined(VK_EXT_opacity_micromap)
            functionTable.vkBuildMicromapsEXT                 = reinterpret_cast<PFN_vkBuildMicromapsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkBuildMicromapsEXT"));
            functionTable.vkCmdBuildMicromapsEXT              = reinterpret_cast<PFN_vkCmdBuildMicromapsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildMicromapsEXT"));
            functionTable.vkCmdCopyMemoryToMicromapEXT        = reinterpret_cast<PFN_vkCmdCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMemoryToMicromapEXT"));
            functionTable.vkCmdCopyMicromapEXT                = reinterpret_cast<PFN_vkCmdCopyMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMicromapEXT"));
            functionTable.vkCmdCopyMicromapToMemoryEXT        = reinterpret_cast<PFN_vkCmdCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMicromapToMemoryEXT"));
            functionTable.vkCmdWriteMicromapsPropertiesEXT    = reinterpret_cast<PFN_vkCmdWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteMicromapsPropertiesEXT"));
            functionTable.vkCopyMemoryToMicromapEXT           = reinterpret_cast<PFN_vkCopyMemoryToMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyMemoryToMicromapEXT"));
            functionTable.vkCopyMicromapEXT                   = reinterpret_cast<PFN_vkCopyMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyMicromapEXT"));
            functionTable.vkCopyMicromapToMemoryEXT           = reinterpret_cast<PFN_vkCopyMicromapToMemoryEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCopyMicromapToMemoryEXT"));
            functionTable.vkCreateMicromapEXT                 = reinterpret_cast<PFN_vkCreateMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCreateMicromapEXT"));
            functionTable.vkDestroyMicromapEXT                = reinterpret_cast<PFN_vkDestroyMicromapEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyMicromapEXT"));
            functionTable.vkGetDeviceMicromapCompatibilityEXT = reinterpret_cast<PFN_vkGetDeviceMicromapCompatibilityEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceMicromapCompatibilityEXT"));
            functionTable.vkGetMicromapBuildSizesEXT          = reinterpret_cast<PFN_vkGetMicromapBuildSizesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetMicromapBuildSizesEXT"));
            functionTable.vkWriteMicromapsPropertiesEXT       = reinterpret_cast<PFN_vkWriteMicromapsPropertiesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkWriteMicromapsPropertiesEXT"));
        #endif
        #if defined(VK_EXT_pageable_device_local_memory)
            functionTable.vkSetDeviceMemoryPriorityEXT = reinterpret_cast<PFN_vkSetDeviceMemoryPriorityEXT>(vkGetDeviceProcAddr(logicalDevice, "vkSetDeviceMemoryPriorityEXT"));
        #endif
        #if defined(VK_EXT_pipeline_properties)
            functionTable.vkGetPipelinePropertiesEXT = reinterpret_cast<PFN_vkGetPipelinePropertiesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelinePropertiesEXT"));
        #endif
        #if defined(VK_EXT_private_data)
            functionTable.vkCreatePrivateDataSlotEXT  = reinterpret_cast<PFN_vkCreatePrivateDataSlotEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCreatePrivateDataSlotEXT"));
            functionTable.vkDestroyPrivateDataSlotEXT = reinterpret_cast<PFN_vkDestroyPrivateDataSlotEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyPrivateDataSlotEXT"));
            functionTable.vkGetPrivateDataEXT         = reinterpret_cast<PFN_vkGetPrivateDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetPrivateDataEXT"));
            functionTable.vkSetPrivateDataEXT         = reinterpret_cast<PFN_vkSetPrivateDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkSetPrivateDataEXT"));
        #endif
        #if defined(VK_EXT_sample_locations)
            functionTable.vkCmdSetSampleLocationsEXT = reinterpret_cast<PFN_vkCmdSetSampleLocationsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetSampleLocationsEXT"));
        #endif
        #if defined(VK_EXT_shader_module_identifier)
            functionTable.vkGetShaderModuleCreateInfoIdentifierEXT = reinterpret_cast<PFN_vkGetShaderModuleCreateInfoIdentifierEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetShaderModuleCreateInfoIdentifierEXT"));
            functionTable.vkGetShaderModuleIdentifierEXT           = reinterpret_cast<PFN_vkGetShaderModuleIdentifierEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetShaderModuleIdentifierEXT"));
        #endif
        #if defined(VK_EXT_shader_object)
            functionTable.vkCmdBindShadersEXT      = reinterpret_cast<PFN_vkCmdBindShadersEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindShadersEXT"));
            functionTable.vkCreateShadersEXT       = reinterpret_cast<PFN_vkCreateShadersEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCreateShadersEXT"));
            functionTable.vkDestroyShaderEXT       = reinterpret_cast<PFN_vkDestroyShaderEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyShaderEXT"));
            functionTable.vkGetShaderBinaryDataEXT = reinterpret_cast<PFN_vkGetShaderBinaryDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetShaderBinaryDataEXT"));
        #endif
        #if defined(VK_EXT_swapchain_maintenance1)
            functionTable.vkReleaseSwapchainImagesEXT = reinterpret_cast<PFN_vkReleaseSwapchainImagesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkReleaseSwapchainImagesEXT"));
        #endif
        #if defined(VK_EXT_transform_feedback)
            functionTable.vkCmdBeginQueryIndexedEXT            = reinterpret_cast<PFN_vkCmdBeginQueryIndexedEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginQueryIndexedEXT"));
            functionTable.vkCmdBeginTransformFeedbackEXT       = reinterpret_cast<PFN_vkCmdBeginTransformFeedbackEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginTransformFeedbackEXT"));
            functionTable.vkCmdBindTransformFeedbackBuffersEXT = reinterpret_cast<PFN_vkCmdBindTransformFeedbackBuffersEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindTransformFeedbackBuffersEXT"));
            functionTable.vkCmdDrawIndirectByteCountEXT        = reinterpret_cast<PFN_vkCmdDrawIndirectByteCountEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndirectByteCountEXT"));
            functionTable.vkCmdEndQueryIndexedEXT              = reinterpret_cast<PFN_vkCmdEndQueryIndexedEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndQueryIndexedEXT"));
            functionTable.vkCmdEndTransformFeedbackEXT         = reinterpret_cast<PFN_vkCmdEndTransformFeedbackEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndTransformFeedbackEXT"));
        #endif
        #if defined(VK_EXT_validation_cache)
            functionTable.vkCreateValidationCacheEXT  = reinterpret_cast<PFN_vkCreateValidationCacheEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCreateValidationCacheEXT"));
            functionTable.vkDestroyValidationCacheEXT = reinterpret_cast<PFN_vkDestroyValidationCacheEXT>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyValidationCacheEXT"));
            functionTable.vkGetValidationCacheDataEXT = reinterpret_cast<PFN_vkGetValidationCacheDataEXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetValidationCacheDataEXT"));
            functionTable.vkMergeValidationCachesEXT  = reinterpret_cast<PFN_vkMergeValidationCachesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkMergeValidationCachesEXT"));
        #endif
        #if defined(VK_FUCHSIA_buffer_collection)
            functionTable.vkCreateBufferCollectionFUCHSIA               = reinterpret_cast<PFN_vkCreateBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkCreateBufferCollectionFUCHSIA"));
            functionTable.vkDestroyBufferCollectionFUCHSIA              = reinterpret_cast<PFN_vkDestroyBufferCollectionFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyBufferCollectionFUCHSIA"));
            functionTable.vkGetBufferCollectionPropertiesFUCHSIA        = reinterpret_cast<PFN_vkGetBufferCollectionPropertiesFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferCollectionPropertiesFUCHSIA"));
            functionTable.vkSetBufferCollectionBufferConstraintsFUCHSIA = reinterpret_cast<PFN_vkSetBufferCollectionBufferConstraintsFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkSetBufferCollectionBufferConstraintsFUCHSIA"));
            functionTable.vkSetBufferCollectionImageConstraintsFUCHSIA  = reinterpret_cast<PFN_vkSetBufferCollectionImageConstraintsFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkSetBufferCollectionImageConstraintsFUCHSIA"));
        #endif
        #if defined(VK_FUCHSIA_external_memory)
            functionTable.vkGetMemoryZirconHandleFUCHSIA           = reinterpret_cast<PFN_vkGetMemoryZirconHandleFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryZirconHandleFUCHSIA"));
            functionTable.vkGetMemoryZirconHandlePropertiesFUCHSIA = reinterpret_cast<PFN_vkGetMemoryZirconHandlePropertiesFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryZirconHandlePropertiesFUCHSIA"));
        #endif
        #if defined(VK_FUCHSIA_external_semaphore)
            functionTable.vkGetSemaphoreZirconHandleFUCHSIA    = reinterpret_cast<PFN_vkGetSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkGetSemaphoreZirconHandleFUCHSIA"));
            functionTable.vkImportSemaphoreZirconHandleFUCHSIA = reinterpret_cast<PFN_vkImportSemaphoreZirconHandleFUCHSIA>(vkGetDeviceProcAddr(logicalDevice, "vkImportSemaphoreZirconHandleFUCHSIA"));
        #endif
        #if defined(VK_GOOGLE_display_timing)
            functionTable.vkGetPastPresentationTimingGOOGLE = reinterpret_cast<PFN_vkGetPastPresentationTimingGOOGLE>(vkGetDeviceProcAddr(logicalDevice, "vkGetPastPresentationTimingGOOGLE"));
            functionTable.vkGetRefreshCycleDurationGOOGLE   = reinterpret_cast<PFN_vkGetRefreshCycleDurationGOOGLE>(vkGetDeviceProcAddr(logicalDevice, "vkGetRefreshCycleDurationGOOGLE"));
        #endif
        #if defined(VK_HUAWEI_cluster_culling_shader)
            functionTable.vkCmdDrawClusterHUAWEI         = reinterpret_cast<PFN_vkCmdDrawClusterHUAWEI>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawClusterHUAWEI"));
            functionTable.vkCmdDrawClusterIndirectHUAWEI = reinterpret_cast<PFN_vkCmdDrawClusterIndirectHUAWEI>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawClusterIndirectHUAWEI"));
        #endif
        #if defined(VK_HUAWEI_invocation_mask)
            functionTable.vkCmdBindInvocationMaskHUAWEI = reinterpret_cast<PFN_vkCmdBindInvocationMaskHUAWEI>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindInvocationMaskHUAWEI"));
        #endif
        #if defined(VK_HUAWEI_subpass_shading)
            functionTable.vkCmdSubpassShadingHUAWEI                       = reinterpret_cast<PFN_vkCmdSubpassShadingHUAWEI>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSubpassShadingHUAWEI"));
            functionTable.vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI = reinterpret_cast<PFN_vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI"));
        #endif
        #if defined(VK_INTEL_performance_query)
            functionTable.vkAcquirePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkAcquirePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkAcquirePerformanceConfigurationINTEL"));
            functionTable.vkCmdSetPerformanceMarkerINTEL          = reinterpret_cast<PFN_vkCmdSetPerformanceMarkerINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPerformanceMarkerINTEL"));
            functionTable.vkCmdSetPerformanceOverrideINTEL        = reinterpret_cast<PFN_vkCmdSetPerformanceOverrideINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPerformanceOverrideINTEL"));
            functionTable.vkCmdSetPerformanceStreamMarkerINTEL    = reinterpret_cast<PFN_vkCmdSetPerformanceStreamMarkerINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPerformanceStreamMarkerINTEL"));
            functionTable.vkGetPerformanceParameterINTEL          = reinterpret_cast<PFN_vkGetPerformanceParameterINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkGetPerformanceParameterINTEL"));
            functionTable.vkInitializePerformanceApiINTEL         = reinterpret_cast<PFN_vkInitializePerformanceApiINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkInitializePerformanceApiINTEL"));
            functionTable.vkQueueSetPerformanceConfigurationINTEL = reinterpret_cast<PFN_vkQueueSetPerformanceConfigurationINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkQueueSetPerformanceConfigurationINTEL"));
            functionTable.vkReleasePerformanceConfigurationINTEL  = reinterpret_cast<PFN_vkReleasePerformanceConfigurationINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkReleasePerformanceConfigurationINTEL"));
            functionTable.vkUninitializePerformanceApiINTEL       = reinterpret_cast<PFN_vkUninitializePerformanceApiINTEL>(vkGetDeviceProcAddr(logicalDevice, "vkUninitializePerformanceApiINTEL"));
        #endif
        #if defined(VK_KHR_acceleration_structure)
            functionTable.vkBuildAccelerationStructuresKHR                 = reinterpret_cast<PFN_vkBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(logicalDevice, "vkBuildAccelerationStructuresKHR"));
            functionTable.vkCmdBuildAccelerationStructuresIndirectKHR      = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresIndirectKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructuresIndirectKHR"));
            functionTable.vkCmdBuildAccelerationStructuresKHR              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructuresKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructuresKHR"));
            functionTable.vkCmdCopyAccelerationStructureKHR                = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyAccelerationStructureKHR"));
            functionTable.vkCmdCopyAccelerationStructureToMemoryKHR        = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyAccelerationStructureToMemoryKHR"));
            functionTable.vkCmdCopyMemoryToAccelerationStructureKHR        = reinterpret_cast<PFN_vkCmdCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMemoryToAccelerationStructureKHR"));
            functionTable.vkCmdWriteAccelerationStructuresPropertiesKHR    = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
            functionTable.vkCopyAccelerationStructureKHR                   = reinterpret_cast<PFN_vkCopyAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCopyAccelerationStructureKHR"));
            functionTable.vkCopyAccelerationStructureToMemoryKHR           = reinterpret_cast<PFN_vkCopyAccelerationStructureToMemoryKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCopyAccelerationStructureToMemoryKHR"));
            functionTable.vkCopyMemoryToAccelerationStructureKHR           = reinterpret_cast<PFN_vkCopyMemoryToAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCopyMemoryToAccelerationStructureKHR"));
            functionTable.vkCreateAccelerationStructureKHR                 = reinterpret_cast<PFN_vkCreateAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateAccelerationStructureKHR"));
            functionTable.vkDestroyAccelerationStructureKHR                = reinterpret_cast<PFN_vkDestroyAccelerationStructureKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyAccelerationStructureKHR"));
            functionTable.vkGetAccelerationStructureBuildSizesKHR          = reinterpret_cast<PFN_vkGetAccelerationStructureBuildSizesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureBuildSizesKHR"));
            functionTable.vkGetAccelerationStructureDeviceAddressKHR       = reinterpret_cast<PFN_vkGetAccelerationStructureDeviceAddressKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureDeviceAddressKHR"));
            functionTable.vkGetDeviceAccelerationStructureCompatibilityKHR = reinterpret_cast<PFN_vkGetDeviceAccelerationStructureCompatibilityKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
            functionTable.vkWriteAccelerationStructuresPropertiesKHR       = reinterpret_cast<PFN_vkWriteAccelerationStructuresPropertiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkWriteAccelerationStructuresPropertiesKHR"));
        #endif
        #if defined(VK_KHR_bind_memory2)
            functionTable.vkBindBufferMemory2KHR = reinterpret_cast<PFN_vkBindBufferMemory2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkBindBufferMemory2KHR"));
            functionTable.vkBindImageMemory2KHR  = reinterpret_cast<PFN_vkBindImageMemory2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkBindImageMemory2KHR"));
        #endif
        #if defined(VK_KHR_buffer_device_address)
            functionTable.vkGetBufferDeviceAddressKHR              = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferDeviceAddressKHR"));
            functionTable.vkGetBufferOpaqueCaptureAddressKHR       = reinterpret_cast<PFN_vkGetBufferOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferOpaqueCaptureAddressKHR"));
            functionTable.vkGetDeviceMemoryOpaqueCaptureAddressKHR = reinterpret_cast<PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceMemoryOpaqueCaptureAddressKHR"));
        #endif
        #if defined(VK_KHR_copy_commands2)
            functionTable.vkCmdBlitImage2KHR         = reinterpret_cast<PFN_vkCmdBlitImage2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBlitImage2KHR"));
            functionTable.vkCmdCopyBuffer2KHR        = reinterpret_cast<PFN_vkCmdCopyBuffer2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBuffer2KHR"));
            functionTable.vkCmdCopyBufferToImage2KHR = reinterpret_cast<PFN_vkCmdCopyBufferToImage2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyBufferToImage2KHR"));
            functionTable.vkCmdCopyImage2KHR         = reinterpret_cast<PFN_vkCmdCopyImage2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImage2KHR"));
            functionTable.vkCmdCopyImageToBuffer2KHR = reinterpret_cast<PFN_vkCmdCopyImageToBuffer2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyImageToBuffer2KHR"));
            functionTable.vkCmdResolveImage2KHR      = reinterpret_cast<PFN_vkCmdResolveImage2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResolveImage2KHR"));
        #endif
        #if defined(VK_KHR_create_renderpass2)
            functionTable.vkCmdBeginRenderPass2KHR = reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRenderPass2KHR"));
            functionTable.vkCmdEndRenderPass2KHR   = reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRenderPass2KHR"));
            functionTable.vkCmdNextSubpass2KHR     = reinterpret_cast<PFN_vkCmdNextSubpass2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdNextSubpass2KHR"));
            functionTable.vkCreateRenderPass2KHR   = reinterpret_cast<PFN_vkCreateRenderPass2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateRenderPass2KHR"));
        #endif
        #if defined(VK_KHR_deferred_host_operations)
            functionTable.vkCreateDeferredOperationKHR            = reinterpret_cast<PFN_vkCreateDeferredOperationKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateDeferredOperationKHR"));
            functionTable.vkDeferredOperationJoinKHR              = reinterpret_cast<PFN_vkDeferredOperationJoinKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDeferredOperationJoinKHR"));
            functionTable.vkDestroyDeferredOperationKHR           = reinterpret_cast<PFN_vkDestroyDeferredOperationKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDeferredOperationKHR"));
            functionTable.vkGetDeferredOperationMaxConcurrencyKHR = reinterpret_cast<PFN_vkGetDeferredOperationMaxConcurrencyKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeferredOperationMaxConcurrencyKHR"));
            functionTable.vkGetDeferredOperationResultKHR         = reinterpret_cast<PFN_vkGetDeferredOperationResultKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeferredOperationResultKHR"));
        #endif
        #if defined(VK_KHR_descriptor_update_template)
            functionTable.vkCreateDescriptorUpdateTemplateKHR  = reinterpret_cast<PFN_vkCreateDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateDescriptorUpdateTemplateKHR"));
            functionTable.vkDestroyDescriptorUpdateTemplateKHR = reinterpret_cast<PFN_vkDestroyDescriptorUpdateTemplateKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyDescriptorUpdateTemplateKHR"));
            functionTable.vkUpdateDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkUpdateDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(logicalDevice, "vkUpdateDescriptorSetWithTemplateKHR"));
        #endif
        #if defined(VK_KHR_device_group)
            functionTable.vkCmdDispatchBaseKHR                  = reinterpret_cast<PFN_vkCmdDispatchBaseKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDispatchBaseKHR"));
            functionTable.vkCmdSetDeviceMaskKHR                 = reinterpret_cast<PFN_vkCmdSetDeviceMaskKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDeviceMaskKHR"));
            functionTable.vkGetDeviceGroupPeerMemoryFeaturesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPeerMemoryFeaturesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceGroupPeerMemoryFeaturesKHR"));
        #endif
        #if defined(VK_KHR_display_swapchain)
            functionTable.vkCreateSharedSwapchainsKHR = reinterpret_cast<PFN_vkCreateSharedSwapchainsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSharedSwapchainsKHR"));
        #endif
        #if defined(VK_KHR_draw_indirect_count)
            functionTable.vkCmdDrawIndexedIndirectCountKHR = reinterpret_cast<PFN_vkCmdDrawIndexedIndirectCountKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndexedIndirectCountKHR"));
            functionTable.vkCmdDrawIndirectCountKHR        = reinterpret_cast<PFN_vkCmdDrawIndirectCountKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawIndirectCountKHR"));
        #endif
        #if defined(VK_KHR_dynamic_rendering)
            functionTable.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginRenderingKHR"));
            functionTable.vkCmdEndRenderingKHR   = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndRenderingKHR"));
        #endif
        #if defined(VK_KHR_external_fence_fd)
            functionTable.vkGetFenceFdKHR    = reinterpret_cast<PFN_vkGetFenceFdKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetFenceFdKHR"));
            functionTable.vkImportFenceFdKHR = reinterpret_cast<PFN_vkImportFenceFdKHR>(vkGetDeviceProcAddr(logicalDevice, "vkImportFenceFdKHR"));
        #endif
        #if defined(VK_KHR_external_fence_win32)
            functionTable.vkGetFenceWin32HandleKHR    = reinterpret_cast<PFN_vkGetFenceWin32HandleKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetFenceWin32HandleKHR"));
            functionTable.vkImportFenceWin32HandleKHR = reinterpret_cast<PFN_vkImportFenceWin32HandleKHR>(vkGetDeviceProcAddr(logicalDevice, "vkImportFenceWin32HandleKHR"));
        #endif
        #if defined(VK_KHR_external_memory_fd)
            functionTable.vkGetMemoryFdKHR           = reinterpret_cast<PFN_vkGetMemoryFdKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryFdKHR"));
            functionTable.vkGetMemoryFdPropertiesKHR = reinterpret_cast<PFN_vkGetMemoryFdPropertiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryFdPropertiesKHR"));
        #endif
        #if defined(VK_KHR_external_memory_win32)
            functionTable.vkGetMemoryWin32HandleKHR           = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryWin32HandleKHR"));
            functionTable.vkGetMemoryWin32HandlePropertiesKHR = reinterpret_cast<PFN_vkGetMemoryWin32HandlePropertiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryWin32HandlePropertiesKHR"));
        #endif
        #if defined(VK_KHR_external_semaphore_fd)
            functionTable.vkGetSemaphoreFdKHR    = reinterpret_cast<PFN_vkGetSemaphoreFdKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetSemaphoreFdKHR"));
            functionTable.vkImportSemaphoreFdKHR = reinterpret_cast<PFN_vkImportSemaphoreFdKHR>(vkGetDeviceProcAddr(logicalDevice, "vkImportSemaphoreFdKHR"));
        #endif
        #if defined(VK_KHR_external_semaphore_win32)
            functionTable.vkGetSemaphoreWin32HandleKHR    = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetSemaphoreWin32HandleKHR"));
            functionTable.vkImportSemaphoreWin32HandleKHR = reinterpret_cast<PFN_vkImportSemaphoreWin32HandleKHR>(vkGetDeviceProcAddr(logicalDevice, "vkImportSemaphoreWin32HandleKHR"));
        #endif
        #if defined(VK_KHR_fragment_shading_rate)
            functionTable.vkCmdSetFragmentShadingRateKHR = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetFragmentShadingRateKHR"));
        #endif
        #if defined(VK_KHR_get_memory_requirements2)
            functionTable.vkGetBufferMemoryRequirements2KHR      = reinterpret_cast<PFN_vkGetBufferMemoryRequirements2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetBufferMemoryRequirements2KHR"));
            functionTable.vkGetImageMemoryRequirements2KHR       = reinterpret_cast<PFN_vkGetImageMemoryRequirements2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageMemoryRequirements2KHR"));
            functionTable.vkGetImageSparseMemoryRequirements2KHR = reinterpret_cast<PFN_vkGetImageSparseMemoryRequirements2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSparseMemoryRequirements2KHR"));
        #endif
        #if defined(VK_KHR_maintenance1)
            functionTable.vkTrimCommandPoolKHR = reinterpret_cast<PFN_vkTrimCommandPoolKHR>(vkGetDeviceProcAddr(logicalDevice, "vkTrimCommandPoolKHR"));
        #endif
        #if defined(VK_KHR_maintenance3)
            functionTable.vkGetDescriptorSetLayoutSupportKHR = reinterpret_cast<PFN_vkGetDescriptorSetLayoutSupportKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetLayoutSupportKHR"));
        #endif
        #if defined(VK_KHR_maintenance4)
            functionTable.vkGetDeviceBufferMemoryRequirementsKHR      = reinterpret_cast<PFN_vkGetDeviceBufferMemoryRequirementsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceBufferMemoryRequirementsKHR"));
            functionTable.vkGetDeviceImageMemoryRequirementsKHR       = reinterpret_cast<PFN_vkGetDeviceImageMemoryRequirementsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceImageMemoryRequirementsKHR"));
            functionTable.vkGetDeviceImageSparseMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetDeviceImageSparseMemoryRequirementsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceImageSparseMemoryRequirementsKHR"));
        #endif
        #if defined(VK_KHR_maintenance5)
            functionTable.vkCmdBindIndexBuffer2KHR             = reinterpret_cast<PFN_vkCmdBindIndexBuffer2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindIndexBuffer2KHR"));
            functionTable.vkGetDeviceImageSubresourceLayoutKHR = reinterpret_cast<PFN_vkGetDeviceImageSubresourceLayoutKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceImageSubresourceLayoutKHR"));
            functionTable.vkGetImageSubresourceLayout2KHR      = reinterpret_cast<PFN_vkGetImageSubresourceLayout2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSubresourceLayout2KHR"));
            functionTable.vkGetRenderingAreaGranularityKHR     = reinterpret_cast<PFN_vkGetRenderingAreaGranularityKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetRenderingAreaGranularityKHR"));
        #endif
        #if defined(VK_KHR_map_memory2)
            functionTable.vkMapMemory2KHR   = reinterpret_cast<PFN_vkMapMemory2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkMapMemory2KHR"));
            functionTable.vkUnmapMemory2KHR = reinterpret_cast<PFN_vkUnmapMemory2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkUnmapMemory2KHR"));
        #endif
        #if defined(VK_KHR_performance_query)
            functionTable.vkAcquireProfilingLockKHR = reinterpret_cast<PFN_vkAcquireProfilingLockKHR>(vkGetDeviceProcAddr(logicalDevice, "vkAcquireProfilingLockKHR"));
            functionTable.vkReleaseProfilingLockKHR = reinterpret_cast<PFN_vkReleaseProfilingLockKHR>(vkGetDeviceProcAddr(logicalDevice, "vkReleaseProfilingLockKHR"));
        #endif
        #if defined(VK_KHR_pipeline_executable_properties)
            functionTable.vkGetPipelineExecutableInternalRepresentationsKHR = reinterpret_cast<PFN_vkGetPipelineExecutableInternalRepresentationsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineExecutableInternalRepresentationsKHR"));
            functionTable.vkGetPipelineExecutablePropertiesKHR              = reinterpret_cast<PFN_vkGetPipelineExecutablePropertiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineExecutablePropertiesKHR"));
            functionTable.vkGetPipelineExecutableStatisticsKHR              = reinterpret_cast<PFN_vkGetPipelineExecutableStatisticsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineExecutableStatisticsKHR"));
        #endif
        #if defined(VK_KHR_present_wait)
            functionTable.vkWaitForPresentKHR = reinterpret_cast<PFN_vkWaitForPresentKHR>(vkGetDeviceProcAddr(logicalDevice, "vkWaitForPresentKHR"));
        #endif
        #if defined(VK_KHR_push_descriptor)
            functionTable.vkCmdPushDescriptorSetKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPushDescriptorSetKHR"));
        #endif
        #if defined(VK_KHR_ray_tracing_maintenance1) && defined(VK_KHR_ray_tracing_pipeline)
            functionTable.vkCmdTraceRaysIndirect2KHR = reinterpret_cast<PFN_vkCmdTraceRaysIndirect2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdTraceRaysIndirect2KHR"));
        #endif
        #if defined(VK_KHR_ray_tracing_pipeline)
            functionTable.vkCmdSetRayTracingPipelineStackSizeKHR            = reinterpret_cast<PFN_vkCmdSetRayTracingPipelineStackSizeKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRayTracingPipelineStackSizeKHR"));
            functionTable.vkCmdTraceRaysIndirectKHR                         = reinterpret_cast<PFN_vkCmdTraceRaysIndirectKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdTraceRaysIndirectKHR"));
            functionTable.vkCmdTraceRaysKHR                                 = reinterpret_cast<PFN_vkCmdTraceRaysKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdTraceRaysKHR"));
            functionTable.vkCreateRayTracingPipelinesKHR                    = reinterpret_cast<PFN_vkCreateRayTracingPipelinesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateRayTracingPipelinesKHR"));
            functionTable.vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = reinterpret_cast<PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
            functionTable.vkGetRayTracingShaderGroupHandlesKHR              = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetRayTracingShaderGroupHandlesKHR"));
            functionTable.vkGetRayTracingShaderGroupStackSizeKHR            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupStackSizeKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetRayTracingShaderGroupStackSizeKHR"));
        #endif
        #if defined(VK_KHR_sampler_ycbcr_conversion)
            functionTable.vkCreateSamplerYcbcrConversionKHR  = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSamplerYcbcrConversionKHR"));
            functionTable.vkDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroySamplerYcbcrConversionKHR"));
        #endif
        #if defined(VK_KHR_shared_presentable_image)
            functionTable.vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetSwapchainStatusKHR"));
        #endif
        #if defined(VK_KHR_swapchain)
            functionTable.vkAcquireNextImageKHR   = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(logicalDevice, "vkAcquireNextImageKHR"));
            functionTable.vkCreateSwapchainKHR    = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateSwapchainKHR"));
            functionTable.vkDestroySwapchainKHR   = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroySwapchainKHR"));
            functionTable.vkGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetSwapchainImagesKHR"));
            functionTable.vkQueuePresentKHR       = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(logicalDevice, "vkQueuePresentKHR"));
        #endif
        #if defined(VK_KHR_synchronization2)
            functionTable.vkCmdPipelineBarrier2KHR = reinterpret_cast<PFN_vkCmdPipelineBarrier2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPipelineBarrier2KHR"));
            functionTable.vkCmdResetEvent2KHR      = reinterpret_cast<PFN_vkCmdResetEvent2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdResetEvent2KHR"));
            functionTable.vkCmdSetEvent2KHR        = reinterpret_cast<PFN_vkCmdSetEvent2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetEvent2KHR"));
            functionTable.vkCmdWaitEvents2KHR      = reinterpret_cast<PFN_vkCmdWaitEvents2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWaitEvents2KHR"));
            functionTable.vkCmdWriteTimestamp2KHR  = reinterpret_cast<PFN_vkCmdWriteTimestamp2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteTimestamp2KHR"));
            functionTable.vkQueueSubmit2KHR        = reinterpret_cast<PFN_vkQueueSubmit2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkQueueSubmit2KHR"));
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_AMD_buffer_marker)
            functionTable.vkCmdWriteBufferMarker2AMD = reinterpret_cast<PFN_vkCmdWriteBufferMarker2AMD>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteBufferMarker2AMD"));
        #endif
        #if defined(VK_KHR_synchronization2) && defined(VK_NV_device_diagnostic_checkpoints)
            functionTable.vkGetQueueCheckpointData2NV = reinterpret_cast<PFN_vkGetQueueCheckpointData2NV>(vkGetDeviceProcAddr(logicalDevice, "vkGetQueueCheckpointData2NV"));
        #endif
        #if defined(VK_KHR_timeline_semaphore)
            functionTable.vkGetSemaphoreCounterValueKHR = reinterpret_cast<PFN_vkGetSemaphoreCounterValueKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetSemaphoreCounterValueKHR"));
            functionTable.vkSignalSemaphoreKHR          = reinterpret_cast<PFN_vkSignalSemaphoreKHR>(vkGetDeviceProcAddr(logicalDevice, "vkSignalSemaphoreKHR"));
            functionTable.vkWaitSemaphoresKHR           = reinterpret_cast<PFN_vkWaitSemaphoresKHR>(vkGetDeviceProcAddr(logicalDevice, "vkWaitSemaphoresKHR"));
        #endif
        #if defined(VK_KHR_video_decode_queue)
            functionTable.vkCmdDecodeVideoKHR = reinterpret_cast<PFN_vkCmdDecodeVideoKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDecodeVideoKHR"));
        #endif
        #if defined(VK_KHR_video_queue)
            functionTable.vkBindVideoSessionMemoryKHR            = reinterpret_cast<PFN_vkBindVideoSessionMemoryKHR>(vkGetDeviceProcAddr(logicalDevice, "vkBindVideoSessionMemoryKHR"));
            functionTable.vkCmdBeginVideoCodingKHR               = reinterpret_cast<PFN_vkCmdBeginVideoCodingKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBeginVideoCodingKHR"));
            functionTable.vkCmdControlVideoCodingKHR             = reinterpret_cast<PFN_vkCmdControlVideoCodingKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdControlVideoCodingKHR"));
            functionTable.vkCmdEndVideoCodingKHR                 = reinterpret_cast<PFN_vkCmdEndVideoCodingKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdEndVideoCodingKHR"));
            functionTable.vkCreateVideoSessionKHR                = reinterpret_cast<PFN_vkCreateVideoSessionKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateVideoSessionKHR"));
            functionTable.vkCreateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkCreateVideoSessionParametersKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCreateVideoSessionParametersKHR"));
            functionTable.vkDestroyVideoSessionKHR               = reinterpret_cast<PFN_vkDestroyVideoSessionKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyVideoSessionKHR"));
            functionTable.vkDestroyVideoSessionParametersKHR     = reinterpret_cast<PFN_vkDestroyVideoSessionParametersKHR>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyVideoSessionParametersKHR"));
            functionTable.vkGetVideoSessionMemoryRequirementsKHR = reinterpret_cast<PFN_vkGetVideoSessionMemoryRequirementsKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetVideoSessionMemoryRequirementsKHR"));
            functionTable.vkUpdateVideoSessionParametersKHR      = reinterpret_cast<PFN_vkUpdateVideoSessionParametersKHR>(vkGetDeviceProcAddr(logicalDevice, "vkUpdateVideoSessionParametersKHR"));
        #endif
        #if defined(VK_NVX_binary_import)
            functionTable.vkCmdCuLaunchKernelNVX = reinterpret_cast<PFN_vkCmdCuLaunchKernelNVX>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCuLaunchKernelNVX"));
            functionTable.vkCreateCuFunctionNVX  = reinterpret_cast<PFN_vkCreateCuFunctionNVX>(vkGetDeviceProcAddr(logicalDevice, "vkCreateCuFunctionNVX"));
            functionTable.vkCreateCuModuleNVX    = reinterpret_cast<PFN_vkCreateCuModuleNVX>(vkGetDeviceProcAddr(logicalDevice, "vkCreateCuModuleNVX"));
            functionTable.vkDestroyCuFunctionNVX = reinterpret_cast<PFN_vkDestroyCuFunctionNVX>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyCuFunctionNVX"));
            functionTable.vkDestroyCuModuleNVX   = reinterpret_cast<PFN_vkDestroyCuModuleNVX>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyCuModuleNVX"));
        #endif
        #if defined(VK_NVX_image_view_handle)
            functionTable.vkGetImageViewAddressNVX = reinterpret_cast<PFN_vkGetImageViewAddressNVX>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageViewAddressNVX"));
            functionTable.vkGetImageViewHandleNVX  = reinterpret_cast<PFN_vkGetImageViewHandleNVX>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageViewHandleNVX"));
        #endif
        #if defined(VK_NV_clip_space_w_scaling)
            functionTable.vkCmdSetViewportWScalingNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportWScalingNV"));
        #endif
        #if defined(VK_NV_copy_memory_indirect)
            functionTable.vkCmdCopyMemoryIndirectNV        = reinterpret_cast<PFN_vkCmdCopyMemoryIndirectNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMemoryIndirectNV"));
            functionTable.vkCmdCopyMemoryToImageIndirectNV = reinterpret_cast<PFN_vkCmdCopyMemoryToImageIndirectNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyMemoryToImageIndirectNV"));
        #endif
        #if defined(VK_NV_device_diagnostic_checkpoints)
            functionTable.vkCmdSetCheckpointNV       = reinterpret_cast<PFN_vkCmdSetCheckpointNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCheckpointNV"));
            functionTable.vkGetQueueCheckpointDataNV = reinterpret_cast<PFN_vkGetQueueCheckpointDataNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetQueueCheckpointDataNV"));
        #endif
        #if defined(VK_NV_device_generated_commands)
            functionTable.vkCmdBindPipelineShaderGroupNV             = reinterpret_cast<PFN_vkCmdBindPipelineShaderGroupNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindPipelineShaderGroupNV"));
            functionTable.vkCmdExecuteGeneratedCommandsNV            = reinterpret_cast<PFN_vkCmdExecuteGeneratedCommandsNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdExecuteGeneratedCommandsNV"));
            functionTable.vkCmdPreprocessGeneratedCommandsNV         = reinterpret_cast<PFN_vkCmdPreprocessGeneratedCommandsNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPreprocessGeneratedCommandsNV"));
            functionTable.vkCreateIndirectCommandsLayoutNV           = reinterpret_cast<PFN_vkCreateIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(logicalDevice, "vkCreateIndirectCommandsLayoutNV"));
            functionTable.vkDestroyIndirectCommandsLayoutNV          = reinterpret_cast<PFN_vkDestroyIndirectCommandsLayoutNV>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyIndirectCommandsLayoutNV"));
            functionTable.vkGetGeneratedCommandsMemoryRequirementsNV = reinterpret_cast<PFN_vkGetGeneratedCommandsMemoryRequirementsNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetGeneratedCommandsMemoryRequirementsNV"));
        #endif
        #if defined(VK_NV_device_generated_commands_compute)
            functionTable.vkCmdUpdatePipelineIndirectBufferNV       = reinterpret_cast<PFN_vkCmdUpdatePipelineIndirectBufferNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdUpdatePipelineIndirectBufferNV"));
            functionTable.vkGetPipelineIndirectDeviceAddressNV      = reinterpret_cast<PFN_vkGetPipelineIndirectDeviceAddressNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineIndirectDeviceAddressNV"));
            functionTable.vkGetPipelineIndirectMemoryRequirementsNV = reinterpret_cast<PFN_vkGetPipelineIndirectMemoryRequirementsNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetPipelineIndirectMemoryRequirementsNV"));
        #endif
        #if defined(VK_NV_external_memory_rdma)
            functionTable.vkGetMemoryRemoteAddressNV = reinterpret_cast<PFN_vkGetMemoryRemoteAddressNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryRemoteAddressNV"));
        #endif
        #if defined(VK_NV_external_memory_win32)
            functionTable.vkGetMemoryWin32HandleNV = reinterpret_cast<PFN_vkGetMemoryWin32HandleNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetMemoryWin32HandleNV"));
        #endif
        #if defined(VK_NV_fragment_shading_rate_enums)
            functionTable.vkCmdSetFragmentShadingRateEnumNV = reinterpret_cast<PFN_vkCmdSetFragmentShadingRateEnumNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetFragmentShadingRateEnumNV"));
        #endif
        #if defined(VK_NV_memory_decompression)
            functionTable.vkCmdDecompressMemoryIndirectCountNV = reinterpret_cast<PFN_vkCmdDecompressMemoryIndirectCountNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDecompressMemoryIndirectCountNV"));
            functionTable.vkCmdDecompressMemoryNV              = reinterpret_cast<PFN_vkCmdDecompressMemoryNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDecompressMemoryNV"));
        #endif
        #if defined(VK_NV_mesh_shader)
            functionTable.vkCmdDrawMeshTasksIndirectCountNV = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectCountNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksIndirectCountNV"));
            functionTable.vkCmdDrawMeshTasksIndirectNV      = reinterpret_cast<PFN_vkCmdDrawMeshTasksIndirectNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksIndirectNV"));
            functionTable.vkCmdDrawMeshTasksNV              = reinterpret_cast<PFN_vkCmdDrawMeshTasksNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdDrawMeshTasksNV"));
        #endif
        #if defined(VK_NV_optical_flow)
            functionTable.vkBindOpticalFlowSessionImageNV = reinterpret_cast<PFN_vkBindOpticalFlowSessionImageNV>(vkGetDeviceProcAddr(logicalDevice, "vkBindOpticalFlowSessionImageNV"));
            functionTable.vkCmdOpticalFlowExecuteNV       = reinterpret_cast<PFN_vkCmdOpticalFlowExecuteNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdOpticalFlowExecuteNV"));
            functionTable.vkCreateOpticalFlowSessionNV    = reinterpret_cast<PFN_vkCreateOpticalFlowSessionNV>(vkGetDeviceProcAddr(logicalDevice, "vkCreateOpticalFlowSessionNV"));
            functionTable.vkDestroyOpticalFlowSessionNV   = reinterpret_cast<PFN_vkDestroyOpticalFlowSessionNV>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyOpticalFlowSessionNV"));
        #endif
        #if defined(VK_NV_ray_tracing)
            functionTable.vkBindAccelerationStructureMemoryNV            = reinterpret_cast<PFN_vkBindAccelerationStructureMemoryNV>(vkGetDeviceProcAddr(logicalDevice, "vkBindAccelerationStructureMemoryNV"));
            functionTable.vkCmdBuildAccelerationStructureNV              = reinterpret_cast<PFN_vkCmdBuildAccelerationStructureNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBuildAccelerationStructureNV"));
            functionTable.vkCmdCopyAccelerationStructureNV               = reinterpret_cast<PFN_vkCmdCopyAccelerationStructureNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdCopyAccelerationStructureNV"));
            functionTable.vkCmdTraceRaysNV                               = reinterpret_cast<PFN_vkCmdTraceRaysNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdTraceRaysNV"));
            functionTable.vkCmdWriteAccelerationStructuresPropertiesNV   = reinterpret_cast<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdWriteAccelerationStructuresPropertiesNV"));
            functionTable.vkCompileDeferredNV                            = reinterpret_cast<PFN_vkCompileDeferredNV>(vkGetDeviceProcAddr(logicalDevice, "vkCompileDeferredNV"));
            functionTable.vkCreateAccelerationStructureNV                = reinterpret_cast<PFN_vkCreateAccelerationStructureNV>(vkGetDeviceProcAddr(logicalDevice, "vkCreateAccelerationStructureNV"));
            functionTable.vkCreateRayTracingPipelinesNV                  = reinterpret_cast<PFN_vkCreateRayTracingPipelinesNV>(vkGetDeviceProcAddr(logicalDevice, "vkCreateRayTracingPipelinesNV"));
            functionTable.vkDestroyAccelerationStructureNV               = reinterpret_cast<PFN_vkDestroyAccelerationStructureNV>(vkGetDeviceProcAddr(logicalDevice, "vkDestroyAccelerationStructureNV"));
            functionTable.vkGetAccelerationStructureHandleNV             = reinterpret_cast<PFN_vkGetAccelerationStructureHandleNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureHandleNV"));
            functionTable.vkGetAccelerationStructureMemoryRequirementsNV = reinterpret_cast<PFN_vkGetAccelerationStructureMemoryRequirementsNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetAccelerationStructureMemoryRequirementsNV"));
            functionTable.vkGetRayTracingShaderGroupHandlesNV            = reinterpret_cast<PFN_vkGetRayTracingShaderGroupHandlesNV>(vkGetDeviceProcAddr(logicalDevice, "vkGetRayTracingShaderGroupHandlesNV"));
        #endif
        #if defined(VK_NV_scissor_exclusive) && VK_NV_SCISSOR_EXCLUSIVE_SPEC_VERSION >= 2
            functionTable.vkCmdSetExclusiveScissorEnableNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetExclusiveScissorEnableNV"));
        #endif
        #if defined(VK_NV_scissor_exclusive)
            functionTable.vkCmdSetExclusiveScissorNV = reinterpret_cast<PFN_vkCmdSetExclusiveScissorNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetExclusiveScissorNV"));
        #endif
        #if defined(VK_NV_shading_rate_image)
            functionTable.vkCmdBindShadingRateImageNV          = reinterpret_cast<PFN_vkCmdBindShadingRateImageNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindShadingRateImageNV"));
            functionTable.vkCmdSetCoarseSampleOrderNV          = reinterpret_cast<PFN_vkCmdSetCoarseSampleOrderNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoarseSampleOrderNV"));
            functionTable.vkCmdSetViewportShadingRatePaletteNV = reinterpret_cast<PFN_vkCmdSetViewportShadingRatePaletteNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportShadingRatePaletteNV"));
        #endif
        #if defined(VK_QCOM_tile_properties)
            functionTable.vkGetDynamicRenderingTilePropertiesQCOM = reinterpret_cast<PFN_vkGetDynamicRenderingTilePropertiesQCOM>(vkGetDeviceProcAddr(logicalDevice, "vkGetDynamicRenderingTilePropertiesQCOM"));
            functionTable.vkGetFramebufferTilePropertiesQCOM      = reinterpret_cast<PFN_vkGetFramebufferTilePropertiesQCOM>(vkGetDeviceProcAddr(logicalDevice, "vkGetFramebufferTilePropertiesQCOM"));
        #endif
        #if defined(VK_QNX_external_memory_screen_buffer)
            functionTable.vkGetScreenBufferPropertiesQNX = reinterpret_cast<PFN_vkGetScreenBufferPropertiesQNX>(vkGetDeviceProcAddr(logicalDevice, "vkGetScreenBufferPropertiesQNX"));
        #endif
        #if defined(VK_VALVE_descriptor_set_host_mapping)
            functionTable.vkGetDescriptorSetHostMappingVALVE           = reinterpret_cast<PFN_vkGetDescriptorSetHostMappingVALVE>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetHostMappingVALVE"));
            functionTable.vkGetDescriptorSetLayoutHostMappingInfoVALVE = reinterpret_cast<PFN_vkGetDescriptorSetLayoutHostMappingInfoVALVE>(vkGetDeviceProcAddr(logicalDevice, "vkGetDescriptorSetLayoutHostMappingInfoVALVE"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state)) || (defined(VK_EXT_shader_object))
            functionTable.vkCmdBindVertexBuffers2EXT       = reinterpret_cast<PFN_vkCmdBindVertexBuffers2EXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdBindVertexBuffers2EXT"));
            functionTable.vkCmdSetCullModeEXT              = reinterpret_cast<PFN_vkCmdSetCullModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCullModeEXT"));
            functionTable.vkCmdSetDepthBoundsTestEnableEXT = reinterpret_cast<PFN_vkCmdSetDepthBoundsTestEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBoundsTestEnableEXT"));
            functionTable.vkCmdSetDepthCompareOpEXT        = reinterpret_cast<PFN_vkCmdSetDepthCompareOpEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthCompareOpEXT"));
            functionTable.vkCmdSetDepthTestEnableEXT       = reinterpret_cast<PFN_vkCmdSetDepthTestEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthTestEnableEXT"));
            functionTable.vkCmdSetDepthWriteEnableEXT      = reinterpret_cast<PFN_vkCmdSetDepthWriteEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthWriteEnableEXT"));
            functionTable.vkCmdSetFrontFaceEXT             = reinterpret_cast<PFN_vkCmdSetFrontFaceEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetFrontFaceEXT"));
            functionTable.vkCmdSetPrimitiveTopologyEXT     = reinterpret_cast<PFN_vkCmdSetPrimitiveTopologyEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPrimitiveTopologyEXT"));
            functionTable.vkCmdSetScissorWithCountEXT      = reinterpret_cast<PFN_vkCmdSetScissorWithCountEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetScissorWithCountEXT"));
            functionTable.vkCmdSetStencilOpEXT             = reinterpret_cast<PFN_vkCmdSetStencilOpEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilOpEXT"));
            functionTable.vkCmdSetStencilTestEnableEXT     = reinterpret_cast<PFN_vkCmdSetStencilTestEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetStencilTestEnableEXT"));
            functionTable.vkCmdSetViewportWithCountEXT     = reinterpret_cast<PFN_vkCmdSetViewportWithCountEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportWithCountEXT"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state2)) || (defined(VK_EXT_shader_object))
            functionTable.vkCmdSetDepthBiasEnableEXT         = reinterpret_cast<PFN_vkCmdSetDepthBiasEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthBiasEnableEXT"));
            functionTable.vkCmdSetLogicOpEXT                 = reinterpret_cast<PFN_vkCmdSetLogicOpEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLogicOpEXT"));
            functionTable.vkCmdSetPatchControlPointsEXT      = reinterpret_cast<PFN_vkCmdSetPatchControlPointsEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPatchControlPointsEXT"));
            functionTable.vkCmdSetPrimitiveRestartEnableEXT  = reinterpret_cast<PFN_vkCmdSetPrimitiveRestartEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPrimitiveRestartEnableEXT"));
            functionTable.vkCmdSetRasterizerDiscardEnableEXT = reinterpret_cast<PFN_vkCmdSetRasterizerDiscardEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRasterizerDiscardEnableEXT"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3)) || (defined(VK_EXT_shader_object))
            functionTable.vkCmdSetAlphaToCoverageEnableEXT            = reinterpret_cast<PFN_vkCmdSetAlphaToCoverageEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetAlphaToCoverageEnableEXT"));
            functionTable.vkCmdSetAlphaToOneEnableEXT                 = reinterpret_cast<PFN_vkCmdSetAlphaToOneEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetAlphaToOneEnableEXT"));
            functionTable.vkCmdSetColorBlendAdvancedEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendAdvancedEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetColorBlendAdvancedEXT"));
            functionTable.vkCmdSetColorBlendEnableEXT                 = reinterpret_cast<PFN_vkCmdSetColorBlendEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetColorBlendEnableEXT"));
            functionTable.vkCmdSetColorBlendEquationEXT               = reinterpret_cast<PFN_vkCmdSetColorBlendEquationEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetColorBlendEquationEXT"));
            functionTable.vkCmdSetColorWriteMaskEXT                   = reinterpret_cast<PFN_vkCmdSetColorWriteMaskEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetColorWriteMaskEXT"));
            functionTable.vkCmdSetConservativeRasterizationModeEXT    = reinterpret_cast<PFN_vkCmdSetConservativeRasterizationModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetConservativeRasterizationModeEXT"));
            functionTable.vkCmdSetDepthClampEnableEXT                 = reinterpret_cast<PFN_vkCmdSetDepthClampEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthClampEnableEXT"));
            functionTable.vkCmdSetDepthClipEnableEXT                  = reinterpret_cast<PFN_vkCmdSetDepthClipEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthClipEnableEXT"));
            functionTable.vkCmdSetDepthClipNegativeOneToOneEXT        = reinterpret_cast<PFN_vkCmdSetDepthClipNegativeOneToOneEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetDepthClipNegativeOneToOneEXT"));
            functionTable.vkCmdSetExtraPrimitiveOverestimationSizeEXT = reinterpret_cast<PFN_vkCmdSetExtraPrimitiveOverestimationSizeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetExtraPrimitiveOverestimationSizeEXT"));
            functionTable.vkCmdSetLineRasterizationModeEXT            = reinterpret_cast<PFN_vkCmdSetLineRasterizationModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLineRasterizationModeEXT"));
            functionTable.vkCmdSetLineStippleEnableEXT                = reinterpret_cast<PFN_vkCmdSetLineStippleEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLineStippleEnableEXT"));
            functionTable.vkCmdSetLogicOpEnableEXT                    = reinterpret_cast<PFN_vkCmdSetLogicOpEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetLogicOpEnableEXT"));
            functionTable.vkCmdSetPolygonModeEXT                      = reinterpret_cast<PFN_vkCmdSetPolygonModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetPolygonModeEXT"));
            functionTable.vkCmdSetProvokingVertexModeEXT              = reinterpret_cast<PFN_vkCmdSetProvokingVertexModeEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetProvokingVertexModeEXT"));
            functionTable.vkCmdSetRasterizationSamplesEXT             = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRasterizationSamplesEXT"));
            functionTable.vkCmdSetRasterizationStreamEXT              = reinterpret_cast<PFN_vkCmdSetRasterizationStreamEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRasterizationStreamEXT"));
            functionTable.vkCmdSetSampleLocationsEnableEXT            = reinterpret_cast<PFN_vkCmdSetSampleLocationsEnableEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetSampleLocationsEnableEXT"));
            functionTable.vkCmdSetSampleMaskEXT                       = reinterpret_cast<PFN_vkCmdSetSampleMaskEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetSampleMaskEXT"));
            functionTable.vkCmdSetTessellationDomainOriginEXT         = reinterpret_cast<PFN_vkCmdSetTessellationDomainOriginEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetTessellationDomainOriginEXT"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_clip_space_w_scaling)) || (defined(VK_EXT_shader_object) && defined(VK_NV_clip_space_w_scaling))
            functionTable.vkCmdSetViewportWScalingEnableNV = reinterpret_cast<PFN_vkCmdSetViewportWScalingEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportWScalingEnableNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_viewport_swizzle)) || (defined(VK_EXT_shader_object) && defined(VK_NV_viewport_swizzle))
            functionTable.vkCmdSetViewportSwizzleNV = reinterpret_cast<PFN_vkCmdSetViewportSwizzleNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetViewportSwizzleNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_fragment_coverage_to_color)) || (defined(VK_EXT_shader_object) && defined(VK_NV_fragment_coverage_to_color))
            functionTable.vkCmdSetCoverageToColorEnableNV   = reinterpret_cast<PFN_vkCmdSetCoverageToColorEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageToColorEnableNV"));
            functionTable.vkCmdSetCoverageToColorLocationNV = reinterpret_cast<PFN_vkCmdSetCoverageToColorLocationNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageToColorLocationNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_framebuffer_mixed_samples)) || (defined(VK_EXT_shader_object) && defined(VK_NV_framebuffer_mixed_samples))
            functionTable.vkCmdSetCoverageModulationModeNV        = reinterpret_cast<PFN_vkCmdSetCoverageModulationModeNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageModulationModeNV"));
            functionTable.vkCmdSetCoverageModulationTableEnableNV = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageModulationTableEnableNV"));
            functionTable.vkCmdSetCoverageModulationTableNV       = reinterpret_cast<PFN_vkCmdSetCoverageModulationTableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageModulationTableNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_shading_rate_image)) || (defined(VK_EXT_shader_object) && defined(VK_NV_shading_rate_image))
            functionTable.vkCmdSetShadingRateImageEnableNV = reinterpret_cast<PFN_vkCmdSetShadingRateImageEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetShadingRateImageEnableNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_representative_fragment_test)) || (defined(VK_EXT_shader_object) && defined(VK_NV_representative_fragment_test))
            functionTable.vkCmdSetRepresentativeFragmentTestEnableNV = reinterpret_cast<PFN_vkCmdSetRepresentativeFragmentTestEnableNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetRepresentativeFragmentTestEnableNV"));
        #endif
        #if (defined(VK_EXT_extended_dynamic_state3) && defined(VK_NV_coverage_reduction_mode)) || (defined(VK_EXT_shader_object) && defined(VK_NV_coverage_reduction_mode))
            functionTable.vkCmdSetCoverageReductionModeNV = reinterpret_cast<PFN_vkCmdSetCoverageReductionModeNV>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetCoverageReductionModeNV"));
        #endif
        #if (defined(VK_EXT_full_screen_exclusive) && defined(VK_KHR_device_group)) || (defined(VK_EXT_full_screen_exclusive) && defined(VK_VERSION_1_1))
            functionTable.vkGetDeviceGroupSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModes2EXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceGroupSurfacePresentModes2EXT"));
        #endif
        #if (defined(VK_EXT_host_image_copy)) || (defined(VK_EXT_image_compression_control))
            functionTable.vkGetImageSubresourceLayout2EXT = reinterpret_cast<PFN_vkGetImageSubresourceLayout2EXT>(vkGetDeviceProcAddr(logicalDevice, "vkGetImageSubresourceLayout2EXT"));
        #endif
        #if (defined(VK_EXT_shader_object)) || (defined(VK_EXT_vertex_input_dynamic_state))
            functionTable.vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(vkGetDeviceProcAddr(logicalDevice, "vkCmdSetVertexInputEXT"));
        #endif
        #if (defined(VK_KHR_descriptor_update_template) && defined(VK_KHR_push_descriptor)) || (defined(VK_KHR_push_descriptor) && defined(VK_VERSION_1_1)) || (defined(VK_KHR_push_descriptor) && defined(VK_KHR_descriptor_update_template))
            functionTable.vkCmdPushDescriptorSetWithTemplateKHR = reinterpret_cast<PFN_vkCmdPushDescriptorSetWithTemplateKHR>(vkGetDeviceProcAddr(logicalDevice, "vkCmdPushDescriptorSetWithTemplateKHR"));
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            functionTable.vkGetDeviceGroupPresentCapabilitiesKHR = reinterpret_cast<PFN_vkGetDeviceGroupPresentCapabilitiesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceGroupPresentCapabilitiesKHR"));
            functionTable.vkGetDeviceGroupSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetDeviceGroupSurfacePresentModesKHR>(vkGetDeviceProcAddr(logicalDevice, "vkGetDeviceGroupSurfacePresentModesKHR"));
        #endif
        #if (defined(VK_KHR_device_group) && defined(VK_KHR_swapchain)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
            functionTable.vkAcquireNextImage2KHR = reinterpret_cast<PFN_vkAcquireNextImage2KHR>(vkGetDeviceProcAddr(logicalDevice, "vkAcquireNextImage2KHR"));
        #endif

        // Retrieve general queue
        functionTable.vkGetDeviceQueue(logicalDevice, generalQueueFamily, 0, &generalQueue);

        // Set up shared fence create info
        VkFenceCreateInfo fenceCreateInfo = { };
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        // Create shared fence
        result = functionTable.vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &sharedCommandBufferFence);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create shared command buffer fence of device [{0}]! Error code: {1}!", GetName(), result);

        // Get Vulkan function pointers
        VmaVulkanFunctions vulkanFunctions = { };
        vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vulkanFunctions.vkGetPhysicalDeviceProperties = instance.GetFunctionTable().vkGetPhysicalDeviceProperties;
        vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = instance.GetFunctionTable().vkGetPhysicalDeviceMemoryProperties;
        vulkanFunctions.vkAllocateMemory = functionTable.vkAllocateMemory;
        vulkanFunctions.vkFreeMemory = functionTable.vkFreeMemory;
        vulkanFunctions.vkMapMemory = functionTable.vkMapMemory;
        vulkanFunctions.vkUnmapMemory = functionTable.vkUnmapMemory;
        vulkanFunctions.vkFlushMappedMemoryRanges = functionTable.vkFlushMappedMemoryRanges;
        vulkanFunctions.vkInvalidateMappedMemoryRanges = functionTable.vkInvalidateMappedMemoryRanges;
        vulkanFunctions.vkBindBufferMemory = functionTable.vkBindBufferMemory;
        vulkanFunctions.vkBindImageMemory = functionTable.vkBindImageMemory;
        vulkanFunctions.vkGetBufferMemoryRequirements = functionTable.vkGetBufferMemoryRequirements;
        vulkanFunctions.vkGetImageMemoryRequirements = functionTable.vkGetImageMemoryRequirements;
        vulkanFunctions.vkCreateBuffer = functionTable.vkCreateBuffer;
        vulkanFunctions.vkDestroyBuffer = functionTable.vkDestroyBuffer;
        vulkanFunctions.vkCreateImage = functionTable.vkCreateImage;
        vulkanFunctions.vkDestroyImage = functionTable.vkDestroyImage;
        vulkanFunctions.vkCmdCopyBuffer = functionTable.vkCmdCopyBuffer;
        if (instance.GetAPIVersion() >= VulkanAPIVersion(1, 1, 0))
        {
            vulkanFunctions.vkGetBufferMemoryRequirements2KHR = functionTable.vkGetBufferMemoryRequirements2;
            vulkanFunctions.vkGetImageMemoryRequirements2KHR = functionTable.vkGetImageMemoryRequirements2;
            vulkanFunctions.vkBindBufferMemory2KHR = functionTable.vkBindBufferMemory2;
            vulkanFunctions.vkBindImageMemory2KHR = functionTable.vkBindImageMemory2;
            vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = instance.GetFunctionTable().vkGetPhysicalDeviceMemoryProperties2;
        }
        if (instance.GetAPIVersion() >= VulkanAPIVersion(1, 3, 0))
        {
            vulkanFunctions.vkGetDeviceBufferMemoryRequirements = functionTable.vkGetDeviceBufferMemoryRequirements;
            vulkanFunctions.vkGetDeviceImageMemoryRequirements = functionTable.vkGetDeviceImageMemoryRequirements;
        }

        // Set up allocator create info
        VmaAllocatorCreateInfo vmaCreteInfo = { };
        vmaCreteInfo.instance = instance.GetVulkanInstance();
        vmaCreteInfo.physicalDevice = physicalDevice;
        vmaCreteInfo.device = logicalDevice;
        vmaCreteInfo.vulkanApiVersion = instance.GetAPIVersion();
        vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;
        vmaCreteInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

        // Create allocator
        vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);

        // Create command pool
        VkCommandPoolCreateInfo commandPoolCreateInfo = { };
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = generalQueueFamily;
        result = functionTable.vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create command pool for device [{0}]! Error code: {1}.", GetName(), result);
    }

    /* --- POLLING METHODS --- */

    void VulkanDevice::SubmitCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot, from device [{0}], submit command buffer [{1}] with a graphics API, that is not [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Set up submit info
        VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        // Submit command buffer
        const VkResult result = functionTable.vkQueueSubmit(generalQueue, 1, &submitInfo, VK_NULL_HANDLE);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Submission of command buffer [{0}] from device [{1}] failed! Error code: {2}.", vulkanCommandBuffer.GetName(), GetName(), result);
    }

    void VulkanDevice::SubmitAndWaitCommandBuffer(const std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Vulkan, "[Vulkan]: Cannot, from device [{0}], submit and wait for command buffer [{1}] with a graphics API, that is not [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const VulkanCommandBuffer &vulkanCommandBuffer = static_cast<const VulkanCommandBuffer&>(*commandBuffer);

        // Set up submit info
        VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.GetVulkanCommandBuffer();
        VkSubmitInfo submitInfo = { };
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        // Submit command buffer
        const VkResult result = functionTable.vkQueueSubmit(generalQueue, 1, &submitInfo, sharedCommandBufferFence);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Submission of command buffer [{0}] from device [{1}] failed! Error code: {2}.", vulkanCommandBuffer.GetName(), GetName(), result);

        // Wait for execution
        functionTable.vkWaitForFences(logicalDevice, 1, &sharedCommandBufferFence, VK_TRUE, std::numeric_limits<uint64>::max());
    }

    /* --- GETTER METHODS --- */

    bool VulkanDevice::IsImageConfigurationSupported(const ImageFormat format, const ImageUsage usage) const
    {
        // Get format properties
        VkFormatProperties formatProperties = { };
        vkGetPhysicalDeviceFormatProperties(physicalDevice, VulkanImage::ImageFormatToVkFormat(format), &formatProperties);

        // Check support
        if (usage & ImageUsage::SourceTransfer         && !(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT                )) return false;
        if (usage & ImageUsage::DestinationTransfer    && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT               )) return false;
        if (usage & ImageUsage::Storage                && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Sampled                && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::Filtered               && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::ColorAttachment        && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) return false;
        if (usage & ImageUsage::DepthAttachment        && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT   )) return false;
        if (usage & ImageUsage::InputAttachment        && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT              )) return false;
        if (usage & ImageUsage::TransientAttachment    && !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT           )) return false;
        return true;
    }

    bool VulkanDevice::IsColorSamplingSupported(const ImageSampling sampling) const
    {
        const VkSampleCountFlags countFlags = physicalDeviceProperties.limits.framebufferColorSampleCounts;
        return countFlags & VulkanImage::ImageSamplingToVkSampleCountFlags(sampling);
    }

    bool VulkanDevice::IsDepthSamplingSupported(const ImageSampling sampling) const
    {
        const VkSampleCountFlags countFlags = physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        return countFlags & VulkanImage::ImageSamplingToVkSampleCountFlags(sampling);
    }

    ImageSampling VulkanDevice::GetHighestColorSampling() const
    {
        if (IsColorSamplingSupported(ImageSampling::x64)) return ImageSampling::x64;
        if (IsColorSamplingSupported(ImageSampling::x32)) return ImageSampling::x32;
        if (IsColorSamplingSupported(ImageSampling::x16)) return ImageSampling::x16;
        if (IsColorSamplingSupported(ImageSampling::x8)) return ImageSampling::x8;
        if (IsColorSamplingSupported(ImageSampling::x4)) return ImageSampling::x4;
        if (IsColorSamplingSupported(ImageSampling::x2)) return ImageSampling::x2;
        return ImageSampling::x1;
    }

    ImageSampling VulkanDevice::GetHighestDepthSampling() const
    {
        if (IsDepthSamplingSupported(ImageSampling::x64)) return ImageSampling::x64;
        if (IsDepthSamplingSupported(ImageSampling::x32)) return ImageSampling::x32;
        if (IsDepthSamplingSupported(ImageSampling::x16)) return ImageSampling::x16;
        if (IsDepthSamplingSupported(ImageSampling::x8)) return ImageSampling::x8;
        if (IsDepthSamplingSupported(ImageSampling::x4)) return ImageSampling::x4;
        if (IsDepthSamplingSupported(ImageSampling::x2)) return ImageSampling::x2;
        return ImageSampling::x1;
    }

    bool VulkanDevice::IsExtensionLoaded(const std::string &extensionName) const
    {
        return std::find(loadedExtensions.begin(), loadedExtensions.end(), std::hash<std::string>{}(extensionName)) != loadedExtensions.end();
    }

    /* --- PRIVATE METHODS --- */

    bool VulkanDevice::IsExtensionSupported(const char* extensionName, const std::vector<VkExtensionProperties> &supportedExtensions)
    {
        for (const auto &supportedExtension : supportedExtensions)
        {
            if (strcmp(extensionName, supportedExtension.extensionName) == 0)
            {
                return true;
            }
        }

        return false;
    }

    template<typename T>
    bool VulkanDevice::AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &extensionDataToFree)
    {
        // Check if root extension is found within the supported ones
        bool extensionSupported = IsExtensionSupported(extension.name.c_str(), supportedExtensions);
        if (extensionSupported && extension.data != nullptr)
        {
            PushToPNextChain(&pNextChain, extension.data);
            extensionDataToFree.push_back(extension.data);
        }

        // If root extension is not found, we do not load it
        if (!extensionSupported)
        {
            if (!extension.requiredOnlyIfSupported) SR_WARNING("Device extension [{0}] requested but not supported! Extension will be discarded, but issues may occur if extensions' support is not checked before their usage!", extension.name);

            // Free all data within the extension tree, as, because the current extension is not supported, its dependencies will never be loaded, and thus creating a memory leak
            std::function<void(const DeviceExtension&)> FreeExtensionTreeLambda = [&FreeExtensionTreeLambda](const DeviceExtension &extension)
            {
                std::free(extension.data);
                for (const auto &dependency : extension.dependencies)
                {
                    FreeExtensionTreeLambda(dependency);
                }
            };
            FreeExtensionTreeLambda(extension);

            return false;
        }

        // If found, we then check if all required dependency extensions are supported
        for (const auto &dependencyExtension : extension.dependencies)
        {
            if (!AddExtensionIfSupported(dependencyExtension, extensionList, supportedExtensions, pNextChain, extensionDataToFree))
            {
                SR_WARNING("Device extension [{0}] requires the support of an unsupported extension [{1}]! Extensions will be discarded and the application may continue to run, but issues may occur if extensions' support is not checked before their usage!", extension.name, dependencyExtension.name);
                return false;
            }
        }

        // Add extension to the list
        loadedExtensions.push_back(std::hash<std::string>{}(extension.name));
        extensionList.push_back(extension.name.c_str());
        return true;
    }

    /* --- DESTRUCTOR --- */

    void VulkanDevice::Destroy()
    {
        functionTable.vkDestroyFence(logicalDevice, sharedCommandBufferFence, nullptr);
        vmaDestroyAllocator(vmaAllocator);
        functionTable.vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
        functionTable.vkDestroyDevice(logicalDevice, nullptr);
    }

}

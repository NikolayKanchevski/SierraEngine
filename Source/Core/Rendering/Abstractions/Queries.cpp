//
// Created by Nikolay Kanchevski on 24.12.22.
//

#include "Queries.h"

#include "../Bases/VK.h"

namespace Sierra::Rendering
{

    // ********************* Timestamp Query ********************* \\

    /* --- CONSTRUCTORS --- */

    TimestampQuery::TimestampQuery()
        : index(VK::GetQueryPool()->queryCount)
    {
        VK::GetQueryPool()->queryCount += 2;
    }

    UniquePtr<TimestampQuery> TimestampQuery::Create()
    {
        return std::make_unique<TimestampQuery>();
    }

    /* --- SETTER METHODS --- */

    void TimestampQuery::Begin(const UniquePtr<CommandBuffer> &commandBuffer, const VkPipelineStageFlagBits pipelineStage) const
    {
        vkCmdResetQueryPool(commandBuffer->GetVulkanCommandBuffer(), VK::GetQueryPool()->GetVulkanQueryPool(), index, 2);
        vkCmdWriteTimestamp(commandBuffer->GetVulkanCommandBuffer(), pipelineStage, VK::GetQueryPool()->GetVulkanQueryPool(), index);
    }

    void TimestampQuery::End(const UniquePtr<CommandBuffer> &commandBuffer, const VkPipelineStageFlagBits pipelineStage) const
    {
        vkCmdWriteTimestamp(commandBuffer->GetVulkanCommandBuffer(), pipelineStage, VK::GetQueryPool()->GetVulkanQueryPool(), index + 1);
    }

    /* --- GETTER METHODS --- */

    float TimestampQuery::GetTimeTaken()
    {
        // Get results from query
        VkResult result = vkGetQueryPoolResults(VK::GetLogicalDevice(), VK::GetQueryPool()->GetVulkanQueryPool(), index, 2, UINT64_SIZE * 2, buffer, UINT64_SIZE, VK_QUERY_RESULT_64_BIT);

        // Check for errors
        if (result == VK_NOT_READY)
        {
            return 0.0f;
        }
        else if (result != VK_SUCCESS)
        {
            ASSERT_WARNING("Getting the results from timestamp query with index [{0}] was unsuccessful. Returned time: 0.0f ms.", index);
            return 0.0f;
        }

        return (static_cast<float>(buffer[1] - buffer[0]) * VK::GetDevice()->GetTimestampPeriod()) * 0.000001f;
    }

    // ********************* Query Pool ********************* \\

    QueryPool::QueryPool(const QueryPoolCreateInfo &createInfo)
    {
        // Set up draw time query creation info
        VkQueryPoolCreateInfo queryPoolCreateInfo{};
        queryPoolCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queryPoolCreateInfo.queryCount = createInfo.queryCount;
        queryPoolCreateInfo.queryType = createInfo.queryType;

        // Create the draw time query pool
        VK_ASSERT(
            vkCreateQueryPool(VK::GetLogicalDevice(), &queryPoolCreateInfo, nullptr, &vkQueryPool),
            "Failed to create timestamp query pool"
        );
    }

    UniquePtr<QueryPool> QueryPool::Create(const QueryPoolCreateInfo &createInfo)
    {
        return std::make_unique<QueryPool>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void QueryPool::Destroy()
    {
        vkDestroyQueryPool(VK::GetLogicalDevice(), vkQueryPool, nullptr);
        vkQueryPool = VK_NULL_HANDLE;
    }
}
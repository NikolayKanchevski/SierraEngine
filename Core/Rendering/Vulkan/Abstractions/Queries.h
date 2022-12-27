//
// Created by Nikolay Kanchevski on 24.12.22.
//

#pragma once

#include <memory>
#include <vulkan/vulkan.h>

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    class TimestampQuery
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimestampQuery();
        static std::unique_ptr<TimestampQuery> Create();

        /* --- SETTER METHODS --- */
        void Begin(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        void End(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        /* --- GETTER METHODS --- */
        [[nodiscard]] float GetTimeTaken() const;

        /* --- OPERATORS --- */
        TimestampQuery(const TimestampQuery &) = delete;
        TimestampQuery &operator=(const TimestampQuery &) = delete;

    private:
        uint32_t index;
        uint64_t* buffer = new uint64_t[2];

        static inline const uint32_t UINT64_SIZE = sizeof(uint64_t);

    };

    struct QueryPoolCreateInfo
    {
        uint32_t queryCount = 32;
        VkQueryType queryType = VK_QUERY_TYPE_TIMESTAMP;
    };

    class QueryPool
    {
    public:
        /* --- CONSTRUCTORS --- */
        QueryPool(const QueryPoolCreateInfo &createInfo);
        static std::unique_ptr<QueryPool> Create(QueryPoolCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkQueryPool GetVulkanQueryPool() const { return vkQueryPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        friend class TimestampQuery;
        QueryPool(const QueryPool &) = delete;
        QueryPool &operator=(const QueryPool &) = delete;

    private:
        VkQueryPool vkQueryPool;
        static inline uint32_t queryCount = 0;

    };

}
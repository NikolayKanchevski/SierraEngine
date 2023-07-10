//
// Created by Nikolay Kanchevski on 24.12.22.
//

#pragma once

#include "CommandBuffer.h"

namespace Sierra::Rendering
{
    class TimestampQuery
    {
    public:
        /* --- CONSTRUCTORS --- */
        TimestampQuery();
        static UniquePtr<TimestampQuery> Create();

        /* --- SETTER METHODS --- */
        void Begin(const UniquePtr<CommandBuffer> &commandBuffer, VkPipelineStageFlagBits pipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT) const;
        void End(const UniquePtr<CommandBuffer> &commandBuffer, VkPipelineStageFlagBits pipelineStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] float GetTimeTaken();

        /* --- OPERATORS --- */
        DELETE_COPY(TimestampQuery);

    private:
        uint index;
        uint64 buffer[2] = { };

    };

    struct QueryPoolCreateInfo
    {
        uint queryCount = 32;
        VkQueryType queryType = VK_QUERY_TYPE_TIMESTAMP;
    };

    class QueryPool
    {
    public:
        /* --- CONSTRUCTORS --- */
        QueryPool(const QueryPoolCreateInfo &createInfo);
        static UniquePtr<QueryPool> Create(QueryPoolCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkQueryPool GetVulkanQueryPool() const { return vkQueryPool; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        friend class TimestampQuery;
        DELETE_COPY(QueryPool);

    private:
        VkQueryPool vkQueryPool = VK_NULL_HANDLE;
        static inline uint queryCount = 0;

    };

}
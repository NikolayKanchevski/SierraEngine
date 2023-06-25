//
// Created by Nikolay Kanchevski on 24.06.23.
//

#pragma once

#include "../Vulkan/Abstractions/Image.h"
#include "../Vulkan/Abstractions/Buffer.h"
#include "../Vulkan/Abstractions/ComputePipeline.h"

namespace Sierra::Core::Rendering::Modules
{
    using namespace Core::Rendering::Vulkan;
    using namespace Core::Rendering::Vulkan::Abstractions;

    struct RaycasterCreateInfo
    {
        const UniquePtr<Image> &IDBuffer;
        const UniquePtr<Image> &depthBuffer;
    };

    struct RaycasterOutputData
    {
        Vector3 worldPosition;
        uint entityID;
    };

    class Raycaster
    {
    public:
        /* --- CONSTRUCTORS --- */
        Raycaster(const RaycasterCreateInfo &createInfo);
        static UniquePtr<Raycaster> Create(const RaycasterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void UpdateData(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<Buffer> &uniformBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline entt::entity GetHoveredEntityID() const { return outputBuffer->GetDataAs<RaycasterOutputData>()->entityID == 0 ? entt::null : static_cast<entt::entity>(outputBuffer->GetDataAs<RaycasterOutputData>()->entityID); }
        [[nodiscard]] inline Vector3 GetHoveredPosition() const { return outputBuffer->GetDataAs<RaycasterOutputData>()->worldPosition; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Raycaster);

    private:
        UniquePtr<Buffer> outputBuffer;
        UniquePtr<DescriptorSetLayout> descriptorSetLayout;
        UniquePtr<ComputePipeline> computePipeline;

        struct PushConstant
        {
            Vector2 mousePosition;
        };

    };

}
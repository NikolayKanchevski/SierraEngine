//
// Created by Nikolay Kanchevski on 24.06.23.
//

#pragma once

#include "../Abstractions/Image.h"
#include "../Abstractions/Buffer.h"
#include "../../../Engine/Classes/Entity.h"
#include "../Abstractions/ComputePipeline.h"

namespace Sierra::Rendering::Modules
{

    struct RaycasterCreateInfo
    {
        const UniquePtr<Image> &IDBuffer;
        const UniquePtr<Image> &depthBuffer;
        const UniquePtr<Sampler> &sampler;
    };

    struct RaycasterOutputData
    {
        Vector3 worldPosition;
        uint32 entityID;
    };

    class Raycaster
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Raycaster(const RaycasterCreateInfo &createInfo);
        static UniquePtr<Raycaster> Create(const RaycasterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void UpdateData(const UniquePtr<CommandBuffer> &commandBuffer, const UniquePtr<Buffer> &uniformBuffer);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Engine::Entity GetHoveredEntity() const { return outputBuffer->GetDataAs<RaycasterOutputData>()->entityID == 0 ? Engine::Entity::Null : Engine::Entity(static_cast<entt::entity>(outputBuffer->GetDataAs<RaycasterOutputData>()->entityID)); }
        [[nodiscard]] inline Vector3 GetHoveredPosition() const { return outputBuffer->GetDataAs<RaycasterOutputData>()->worldPosition; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Raycaster);

    private:
        UniquePtr<Buffer> outputBuffer;
        UniquePtr<DescriptorSetLayout> descriptorSetLayout;
        UniquePtr<ComputePipeline> computePipeline;

        const UniquePtr<Image> &IDBuffer;
        const UniquePtr<Image> &depthBuffer;
        const UniquePtr<Sampler> &sampler;

        struct PushConstant
        {
            Vector2 mousePosition;
        };

    };
}
//
// Created by Nikolay Kanchevski on 11.10.22.
//

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InternalComponents.h"
#include "../Structures/Material.h"
#include "../Structures/Vertex.h"
#include "../Classes/Binary.h"

#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"
#include "../../Core/Rendering/Vulkan/VulkanUtilities.h"
#include "../../Core/Rendering/Vulkan/VulkanCore.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Descriptors.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;
using namespace Sierra::Engine::Classes;


namespace Sierra::Engine::Components
{
    struct alignas(16) PushConstant
    {
        Material material;

        uint32_t meshID;
        Binary meshTexturesPresence; // Bools encoded as binary indicating whether texture types are bound
    };

    class Mesh : public Component
    {
    public:
        /* --- PROPERTIES --- */
        static uint32_t totalMeshCount;
        static uint32_t totalMeshVertices;
        Material material{};

        /* --- CONSTRUCTORS --- */
        Mesh() = default;
        Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);

        /* --- SETTER METHODS --- */
        void SetTexture(const std::shared_ptr<Texture>& givenTexture);
        void ResetTexture(TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32_t GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline VkBuffer GetVertexBuffer() const { return vertexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] inline VkBuffer GetIndexBuffer() const { return indexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet->GetVulkanDescriptorSet(); }
        [[nodiscard]] inline uint32_t GetMeshID() const { return this->meshID; }
        [[nodiscard]] glm::mat4x4 GetModelMatrix() const;
        void GetPushConstantData(PushConstant *data) const;

        /* --- DESTRUCTOR --- */
        void Destroy() const override;

    private:
        uint32_t vertexCount;
        uint32_t indexCount;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        std::unique_ptr<DescriptorSet> descriptorSet;
        std::shared_ptr<Texture> textures[TOTAL_TEXTURE_TYPES_COUNT];

        void CreateVertexBuffer(std::vector<Vertex>  &givenVertices);
        void CreateIndexBuffer(std::vector<uint32_t> &givenIndices);
        void CreateDescriptorSet();

        uint32_t meshID;
        Binary meshTexturesPresence = 0;

        static uint32_t meshSlotsUsed;
        static std::vector<uint32_t> freedMeshSlots;
    };

}
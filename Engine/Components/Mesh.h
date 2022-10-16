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

#include "Transform.h"
#include "../Structures/Material.h"
#include "../Structures/Vertex.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"
#include "../../Core/Rendering/Vulkan/VulkanUtilities.h"
#include "../../Core/Rendering/Vulkan/VulkanCore.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Descriptors.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Engine::Components
{

    class Mesh
    {
    public:
        /* --- TESTER --- */
        Transform transform{};

        /* --- PROPERTIES --- */
        static std::vector<std::unique_ptr<Mesh>> worldMeshes;
        static uint32_t totalMeshCount;
        static uint32_t totalMeshVertices;
        Material material{};

        /* --- CONSTRUCTORS --- */
        Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);
        static std::unique_ptr<Mesh>& RegisterMesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);

        struct alignas(16) PushConstantData
        {
            glm::mat4x4 modelMatrix;
            Material material;
        };

        /* --- POLLING METHODS --- */

        /* --- SETTER METHODS --- */
        void SetTexture(const std::shared_ptr<Texture> givenTexture);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32_t GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline VkBuffer GetVertexBuffer() const { return vertexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] inline VkBuffer GetIndexBuffer() const { return indexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet->GetVulkanDescriptorSet(); }
        void GetPushConstantData(PushConstantData *data) const;

        /* --- DESTRUCTOR --- */
        void Destroy();
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

    private:
        uint32_t vertexCount;
        uint32_t indexCount;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        bool descriptorSetCreated = false;
        std::unique_ptr<DescriptorSet> descriptorSet;

        std::shared_ptr<Texture> textures[TOTAL_TEXTURE_TYPES_COUNT];

        void CreateVertexBuffer(std::vector<Vertex>  &givenVertices);
        void CreateIndexBuffer(std::vector<uint32_t> &givenIndices);
        void CreateDescriptorSet();
    };

}
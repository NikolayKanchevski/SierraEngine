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

#include "../Structures/Material.h"
#include "../Structures/Vertex.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Buffer.h"
#include "Transform.h"

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

        /* --- CONSTRUCTORS --- */
        Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);
        static std::unique_ptr<Mesh>& RegisterMesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices);

        struct PushConstantData
        {
            glm::mat4x4 modelMatrix;
//            Material material;
        };

        /* --- POLLING METHODS --- */

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32_t GetIndexCount() const { return indexCount; }
        [[nodiscard]] inline VkBuffer GetVertexBuffer() const { return vertexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] inline VkBuffer GetIndexBuffer() const { return indexBuffer->GetVulkanBuffer(); }
        [[nodiscard]] PushConstantData GetPushConstantData() const;

        /* --- DESTRUCTOR --- */
        void Destroy();
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

    private:
        uint32_t vertexCount;
        uint32_t indexCount;

        std::unique_ptr<Buffer> vertexBuffer;
        std::unique_ptr<Buffer> indexBuffer;

        void CreateVertexBuffer(std::vector<Vertex>  &givenVertices);
        void CreateIndexBuffer(std::vector<uint32_t> &givenIndices);
    };

}
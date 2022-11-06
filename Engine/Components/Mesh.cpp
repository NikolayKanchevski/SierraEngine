//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

using Sierra::Core::Debugger;
using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Components
{
    uint32_t Mesh::totalMeshCount = 0;
    uint32_t Mesh::totalMeshVertices = 0;

    uint32_t Mesh::meshSlotsUsed = 0;
    std::vector<uint32_t> Mesh::freedMeshSlots;

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices)
        : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        if (freedMeshSlots.empty())
        {
            startTextureSlot = meshSlotsUsed * TOTAL_TEXTURE_TYPES_COUNT;
            meshSlotsUsed++;

            if (VulkanCore::GetDescriptorIndexingSupported())
            {
                auto &globalBindlessDescriptorSet = VulkanCore::GetGlobalBindlessDescriptorSet();

                // Reserve spots for mesh's textures
                for (uint32_t i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
                {
                    globalBindlessDescriptorSet->ReserveIndex(BINDLESS_TEXTURE_BINDING, startTextureSlot + i);
                }
            }
        }
        else
        {
            startTextureSlot = freedMeshSlots[0];
            freedMeshSlots.erase(freedMeshSlots.begin());
        }

        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        if (!VulkanCore::GetDescriptorIndexingSupported()) CreateDescriptorSet();

        totalMeshCount++;
        totalMeshVertices += vertexCount;
    }

    /* --- SETTER METHODS --- */

    void Mesh::CreateVertexBuffer(std::vector<Vertex> &givenVertices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(Vertex) * givenVertices.size();

        std::unique_ptr<Buffer> stagingBuffer = Buffer::Create({
            .memorySize = bufferSize,
            .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::Create({
           .memorySize = bufferSize,
           .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
           .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
       });

        stagingBuffer->CopyToBuffer(vertexBuffer);

        stagingBuffer->Destroy();
    }

    void Mesh::CreateIndexBuffer(std::vector<uint32_t> &givenIndices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(uint32_t) * givenIndices.size();

        std::unique_ptr<Buffer> stagingBuffer = Buffer::Create({
              .memorySize = bufferSize,
              .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
              .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        });

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::Create({
             .memorySize = bufferSize,
             .memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
             .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
         });

        stagingBuffer->CopyToBuffer(indexBuffer);

        stagingBuffer->Destroy();
    }

    void Mesh::CreateDescriptorSet()
    {
        // If descriptor indexing not supported write default textures to the corresponding descriptor set
        descriptorSet = DescriptorSet::Build(VulkanCore::GetDescriptorPool());
        descriptorSet->WriteTexture(DIFFUSE_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_DIFFUSE));
        descriptorSet->WriteTexture(SPECULAR_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_SPECULAR));
        descriptorSet->Allocate();
    }

    void Mesh::SetTexture(const std::shared_ptr<Texture>& givenTexture)
    {
        ASSERT_ERROR_IF(givenTexture->GetTextureType() == TEXTURE_TYPE_NONE, "In order to bind texture [" + givenTexture->name + "] to mesh its texture type must be specified and be different from TEXTURE_TYPE_NONE");

        textures[givenTexture->GetTextureType()] = givenTexture;

        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            VulkanCore::GetGlobalBindlessDescriptorSet()->WriteTexture(BINDLESS_TEXTURE_BINDING, givenTexture, true, TOTAL_TEXTURE_TYPES_COUNT + startTextureSlot + (uint32_t) givenTexture->GetTextureType());
            VulkanCore::GetGlobalBindlessDescriptorSet()->Allocate();
        }
        else
        {
            descriptorSet->WriteTexture(TEXTURE_TYPE_TO_BINDING(givenTexture->GetTextureType()), givenTexture);
            descriptorSet->Allocate();
        }

        meshTexturesPresence.SetBit(givenTexture->GetTextureType(), 1);
    }

    void Mesh::ResetTexture(const TextureType textureType)
    {
        ASSERT_ERROR_IF(textureType == TEXTURE_TYPE_NONE, "In order to rest a mesh's texture the texture type must not be TEXTURE_TYPE_NONE");

        textures[textureType] = nullptr;

        if (!VulkanCore::GetDescriptorIndexingSupported())
        {
            descriptorSet->WriteTexture(TEXTURE_TYPE_TO_BINDING(textureType), Texture::GetDefaultTexture(textureType));
            descriptorSet->Allocate();
        }

        meshTexturesPresence.SetBit(textureType, 0);
    }

    /* --- GETTER METHODS --- */

    void Mesh::GetPushConstantData(PushConstant *data) const
    {
        // Inverse the Y coordinate to satisfy Vulkan's requirements
        Transform transform = GetComponent<Transform>();
        glm::vec3 rendererPosition = { transform.position.x, transform.position.y * -1, transform.position.z };

        // Calculate translation matrix
        glm::mat4x4 translationMatrix(1.0);
        translationMatrix = glm::translate(translationMatrix, rendererPosition);

        // Calculate rotation matrix
        glm::mat4x4 rotationMatrix(1.0);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.x), { 0.0, 1.0, 0.0 });
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.y), { 1.0, 0.0, 0.0 });
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.z), { 0.0, 0.0, 1.0 });

        // Calculate scale matrix
        glm::mat4x4 scaleMatrix(1.0);
        scaleMatrix = glm::scale(scaleMatrix, transform.scale);

        // Populate push constant data
        data->modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        data->material = this->material;
        data->meshSlot = this->startTextureSlot;
        data->meshTexturesPresence = this->meshTexturesPresence;
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy() const
    {
        Component::Destroy();

        freedMeshSlots.push_back(startTextureSlot);

        vertexBuffer->Destroy();
        indexBuffer->Destroy();

        for (const auto &texture : textures)
        {
            if (texture == nullptr) continue;
            texture->Destroy();
        }

        totalMeshCount--;
        totalMeshVertices -= vertexCount;
    }
}
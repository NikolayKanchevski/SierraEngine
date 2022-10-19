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

    /* --- CONSTRUCTORS --- */

    Mesh::Mesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices)
        : vertexCount(givenVertices.size()), indexCount(givenIndices.size())
    {
        CreateVertexBuffer(givenVertices);
        CreateIndexBuffer(givenIndices);

        CreateDescriptorSet();

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
        descriptorSet = DescriptorSet::Build(VulkanCore::GetDescriptorPool());
        descriptorSet->WriteTexture(DIFFUSE_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_DIFFUSE));
        descriptorSet->WriteTexture(SPECULAR_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_SPECULAR));
        descriptorSet->Allocate();
    }

    void Mesh::SetTexture(const std::shared_ptr<Texture> givenTexture)
    {
        if (givenTexture->GetTextureType() == TEXTURE_TYPE_NONE)
        {
            Debugger::ThrowError("In order to bind texture [" + givenTexture->name + "] to mesh its texture type must be specified and be different from TEXTURE_TYPE_NONE");
        }

        textures[TextureTypeToArrayIndex(givenTexture->GetTextureType())] = givenTexture;
        descriptorSet->WriteTexture(TextureTypeToBinding(givenTexture->GetTextureType()), givenTexture);
        descriptorSet->Allocate();
    }

    /* --- GETTER METHODS --- */

    void Mesh::GetPushConstantData(Mesh::PushConstantData *data) const
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
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {
        vertexBuffer->Destroy();
        indexBuffer->Destroy();

        for (const auto &texture : textures)
        {
            if (texture == nullptr) continue;
            texture.get()->Destroy();
        }

        totalMeshCount--;
        totalMeshVertices -= vertexCount;
    }
}
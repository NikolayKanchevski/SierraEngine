//
// Created by Nikolay Kanchevski on 11.10.22.
//

#include "Mesh.h"

namespace Sierra::Engine::Components
{
    std::vector<std::unique_ptr<Mesh>> Mesh::worldMeshes;
    uint32_t Mesh::totalMeshCount = 0;
    uint32_t Mesh::totalMeshVertices = 0;

    using namespace Sierra::Core::Rendering::Vulkan;

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

    std::unique_ptr<Mesh> &Mesh::RegisterMesh(std::vector<Vertex> &givenVertices, std::vector<uint32_t> &givenIndices)
    {
        worldMeshes.push_back(std::make_unique<Mesh>(givenVertices, givenIndices));
        return worldMeshes.back();
    }

    /* --- SETTER METHODS --- */

    void Mesh::CreateVertexBuffer(std::vector<Vertex> &givenVertices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(Vertex) * givenVertices.size();

        std::unique_ptr<Buffer> stagingBuffer = Buffer::Builder()
            .SetMemorySize(bufferSize)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .SetUsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        .Build();

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenVertices.data());

        vertexBuffer = Buffer::Builder()
            .SetMemorySize(bufferSize)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .SetUsageFlags(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        .Build();

        stagingBuffer->CopyToBuffer(vertexBuffer);

        stagingBuffer->Destroy();
    }

    void Mesh::CreateIndexBuffer(std::vector<uint32_t> &givenIndices)
    {
        // Calculate the buffer size
        uint64_t bufferSize = sizeof(uint32_t) * givenIndices.size();

        std::unique_ptr<Buffer> stagingBuffer = Buffer::Builder()
            .SetMemorySize(bufferSize)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .SetUsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        .Build();

        // Fill the data pointer with the vertices array's information
        stagingBuffer->CopyFromPointer(givenIndices.data());

        indexBuffer = Buffer::Builder()
            .SetMemorySize(bufferSize)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .SetUsageFlags(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        .Build();

        stagingBuffer->CopyToBuffer(indexBuffer);

        stagingBuffer->Destroy();
    }

    void Mesh::CreateDescriptorSet()
    {

    }

    /* --- GETTER METHODS --- */

    void Mesh::GetPushConstantData(Mesh::PushConstantData *data) const
    {
        // Inverse the Y coordinate to satisfy Vulkan's requirements
        glm::vec3 rendererPosition = { transform.position.x, transform.position.y * -1, transform.position.z };

        // Update the model matrix per call
        glm::mat4x4 translationMatrix(1.0);
        translationMatrix = glm::translate(translationMatrix, { 0, 0, 0 });

        glm::mat4x4 rotationMatrix(1.0);
        glm::rotate(rotationMatrix, glm::radians(transform.rotation.x), { 1.0, 0.0, 0.0 });
        glm::rotate(rotationMatrix, glm::radians(transform.rotation.y), { 0.0, 1.0, 0.0 });
        glm::rotate(rotationMatrix, glm::radians(transform.rotation.z), { 0.0, 0.0, 1.0 });

        glm::mat4x4 scaleMatrix(1.0);
        glm::scale(scaleMatrix, transform.scale);

        data->modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
//        data->modelMatrix = glm::mat4x4(1.0f);
//        pushConstantData.material.shininess = material.shininess;
//        pushConstantData.material.diffuse = material.diffuse;
//        pushConstantData.material.specular = material.specular;
//        pushConstantData.material.ambient = material.ambient;
    }

    /* --- DESTRUCTOR --- */

    void Mesh::Destroy()
    {
        vertexBuffer->Destroy();
        indexBuffer->Destroy();

        totalMeshCount--;
        totalMeshVertices -= vertexCount;
    }
}
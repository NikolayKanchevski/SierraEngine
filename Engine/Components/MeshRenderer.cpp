//
// Created by Nikolay Kanchevski on 20.11.22.
//

#include "MeshRenderer.h"

#include "InternalComponents.h"
#include "../../Core/Rendering/Vulkan/VulkanCore.h"

using Sierra::Core::Rendering::Vulkan::VulkanCore;

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    MeshRenderer::MeshRenderer(std::shared_ptr<Mesh> givenCorrespondingMesh)
        : coreMesh(givenCorrespondingMesh)
    {
        ASSERT_ERROR_IF(meshSlotsUsed >= MAX_MESHES, "Limit of maximum [" + std::to_string(MAX_MESHES) + "] meshes reached");

        // Check if there are any freed slots
        if (freedMeshSlots.empty())
        {
            // Create new mesh slot
            meshID = meshSlotsUsed;
            meshSlotsUsed++;

            #if DEBUG
                // Check if descriptor indexing is supported
                if (VulkanCore::GetDescriptorIndexingSupported())
                {
                    auto &globalBindlessDescriptorSet = VulkanCore::GetGlobalBindlessDescriptorSet();

                    // Reserve spots for mesh's textures
                    for (uint32_t i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
                    {
                        globalBindlessDescriptorSet->ReserveIndex(BINDLESS_TEXTURE_BINDING, meshID + i);
                    }
                }
            #endif
        }
        else
        {
            // Use first freed slot
            meshID = freedMeshSlots[0];
            freedMeshSlots.erase(freedMeshSlots.begin());
        }

        if (!VulkanCore::GetDescriptorIndexingSupported()) CreateDescriptorSet();

        // Assign textures array to use default ones
        for (uint32_t i = TOTAL_TEXTURE_TYPES_COUNT - 1; i--;)
        {
            textures[i] = Texture::GetDefaultTexture((TextureType) i);
        }
    }

    /* --- SETTER METHODS --- */

    void MeshRenderer::CreateDescriptorSet()
    {
        // If descriptor indexing not supported write default textures to the corresponding descriptor set
        descriptorSet = DescriptorSet::Build(VulkanCore::GetDescriptorPool());
        descriptorSet->WriteTexture(DIFFUSE_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_DIFFUSE));
        descriptorSet->WriteTexture(SPECULAR_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_SPECULAR));
        descriptorSet->WriteTexture(HEIGHT_MAP_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_HEIGHT_MAP));
        descriptorSet->Allocate();
    }

    void MeshRenderer::SetTexture(const std::shared_ptr<Texture>& givenTexture)
    {
        ASSERT_ERROR_IF(givenTexture->GetTextureType() == TEXTURE_TYPE_NONE, "In order to bind texture [" + givenTexture->name + "] to mesh its texture type must be specified and be different from TEXTURE_TYPE_NONE");

        textures[givenTexture->GetTextureType()] = givenTexture;

        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            VulkanCore::GetGlobalBindlessDescriptorSet()->WriteTexture(BINDLESS_TEXTURE_BINDING, givenTexture, true, (TOTAL_TEXTURE_TYPES_COUNT * meshID) + (uint32_t) givenTexture->GetTextureType());
            VulkanCore::GetGlobalBindlessDescriptorSet()->Allocate();
        }
        else
        {
            descriptorSet->WriteTexture(TEXTURE_TYPE_TO_BINDING(givenTexture->GetTextureType()), givenTexture);
            descriptorSet->Allocate();
        }

        meshTexturesPresence.SetBit(givenTexture->GetTextureType(), 1);
    }

    void MeshRenderer::ResetTexture(const TextureType textureType)
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

    glm::mat4x4 MeshRenderer::GetModelMatrix() const
    {
        // Inverse the Y coordinate to satisfy Vulkan's requirements
        Transform transform = GetComponent<Transform>();
        glm::vec3 rendererPosition = {transform.position.x, transform.position.y * -1, transform.position.z};

        // Calculate translation matrix
        glm::mat4x4 translationMatrix(1.0);
        translationMatrix = glm::translate(translationMatrix, rendererPosition);

        // Calculate rotation matrix
        glm::mat4x4 rotationMatrix(1.0);
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.x), {0.0, 1.0, 0.0});
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.y), {1.0, 0.0, 0.0});
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(transform.rotation.z), {0.0, 0.0, 1.0});

        // Calculate scale matrix
        glm::mat4x4 scaleMatrix(1.0);
        scaleMatrix = glm::scale(scaleMatrix, transform.scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    PushConstant MeshRenderer::GetPushConstantData() const
    {
        return { .material = material, .meshID = meshID, .meshTexturesPresence =  meshTexturesPresence };
    }

    /* --- DESTRUCTOR --- */

    void MeshRenderer::Destroy() const
    {
        freedMeshSlots.push_back(meshID);

        for (const auto &texture : textures)
        {
            if (texture == nullptr) continue;
            texture->Destroy();
        }

        coreMesh->Destroy();
        RemoveComponent<MeshRenderer>();
    }
}
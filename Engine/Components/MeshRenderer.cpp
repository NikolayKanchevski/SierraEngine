//
// Created by Nikolay Kanchevski on 20.11.22.
//

#include "MeshRenderer.h"

#include "../../Core/Rendering/UI/ImGuiUtilities.h"
#include "Transform.h"

using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    MeshRenderer::MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh)
        : coreMesh(givenCorrespondingMesh)
    {
        ASSERT_ERROR_IF(meshSlotsUsed >= MAX_MESHES, "Limit of maximum [" + std::to_string(MAX_MESHES) + "] meshes reached");

        // Check if there are any freed slots
        if (freedMeshSlots.empty())
        {
            // Create new mesh slot
            meshID = meshSlotsUsed;
            meshSlotsUsed++;

            // TODO: BINDLESS
//            // Check if descriptor indexing is supported
//            if (VK::GetDevice()->GetDescriptorIndexingSupported())
//            {
//                auto &globalBindlessDescriptorSet = VulkanCore::GetGlobalBindlessDescriptorSet();
//
//                // Reserve spots for mesh's textures
//                for (uint i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
//                {
//                    globalBindlessDescriptorSet->ReserveIndex(BINDLESS_TEXTURE_BINDING, meshID + i);
//                }
//            }
        }
        else
        {
            // Use first freed slot
            meshID = freedMeshSlots[0];
            freedMeshSlots.erase(freedMeshSlots.begin());
        }

        if (!VK::GetDevice()->GetDescriptorIndexingSupported()) CreateDescriptorSet();

        // Assign textures array to use default ones
        for (uint i = TOTAL_TEXTURE_TYPES_COUNT; i--;)
        {
            textures[i] = Texture::GetDefaultTexture((TextureType) i);
        }
    }

    /* --- POLLING METHODS --- */

    void MeshRenderer::DrawUI()
    {
        ImGui::BeginProperties();

        if (ImGui::BeginTreeProperties("Material"))
        {
            ImGui::SetInputLimits({ 0.0f, 1.0f });
            ImGui::FloatProperty("Shininess:", material.shininess, "Some tooltip.");
            ImGui::ResetInputLimits();

            ImGui::FloatProperty("Vertex Exaggaration:", material.vertexExaggeration, "Some tooltip.");

            static const float resetValues[3] = { 1.0f, 1.0f, 1.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            ImGui::PropertyVector3("Diffuse:", material.diffuse, resetValues, tooltips);
            ImGui::PropertyVector3("Specular:", material.specular, resetValues, tooltips);
            ImGui::PropertyVector3("Ambient:", material.ambient, resetValues, tooltips);

            ImGui::EndTreeProperties();
        }

        ImGui::TextureProperty("Diffuse Texture:", *this, TEXTURE_TYPE_DIFFUSE, "Some tooltip.");
        ImGui::TextureProperty("Specular Texture:", *this, TEXTURE_TYPE_SPECULAR, "Some tooltip.");
        ImGui::TextureProperty("Height Map Texture:", *this, TEXTURE_TYPE_HEIGHT_MAP, "Some tooltip.");

        ImGui::EndProperties();
    }

    /* --- SETTER METHODS --- */

    void MeshRenderer::CreateDescriptorSet()
    {
        // If descriptor indexing not supported write default textures to the corresponding descriptor set
        descriptorSet = DescriptorSet::Build(VK::GetDescriptorSetLayout());
        descriptorSet->WriteTexture(DIFFUSE_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_DIFFUSE));
        descriptorSet->WriteTexture(SPECULAR_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_SPECULAR));
        descriptorSet->WriteTexture(HEIGHT_MAP_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_HEIGHT_MAP));
        descriptorSet->Allocate();
    }

    void MeshRenderer::SetTexture(const SharedPtr<Texture>& givenTexture)
    {
        ASSERT_ERROR_IF(givenTexture->GetTextureType() == TEXTURE_TYPE_NONE, "In order to bind texture [" + givenTexture->name + "] to mesh its texture type must be specified and be different from TEXTURE_TYPE_NONE");

        textures[givenTexture->GetTextureType()] = givenTexture;

        VK::GetDevice()->WaitUntilIdle();

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
//            VulkanCore::GetGlobalBindlessDescriptorSet()->WriteTexture(BINDLESS_TEXTURE_BINDING, givenTexture, true, (TOTAL_TEXTURE_TYPES_COUNT * meshID) + (uint) givenTexture->GetTextureType());
//            VulkanCore::GetGlobalBindlessDescriptorSet()->Allocate();
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
        ASSERT_ERROR_IF(textureType == TEXTURE_TYPE_NONE, "In order to reset a mesh's texture the texture type must not be TEXTURE_TYPE_NONE");

        SetTexture(Texture::GetDefaultTexture(textureType));

        meshTexturesPresence.SetBit(textureType, 0);
    }

    /* --- GETTER METHODS --- */

    Matrix4x4 MeshRenderer::GetModelMatrix() const
    {
        return GetComponent<Transform>().GetModelMatrix();
    }

    MeshPushConstant MeshRenderer::GetPushConstantData() const
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
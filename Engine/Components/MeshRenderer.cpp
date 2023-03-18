//
// Created by Nikolay Kanchevski on 20.11.22.
//

#include "MeshRenderer.h"

#include "../../Core/Rendering/Vulkan/VK.h"
#include "../../Core/Rendering/RenderingSettings.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"
#include "Transform.h"

using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Components
{

    /* --- CONSTRUCTORS --- */

    MeshRenderer::MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh)
        : coreMesh(givenCorrespondingMesh)
    {
        ASSERT_ERROR_FORMATTED_IF(meshSlotsUsed >= MAX_MESHES, "Limit of maximum [{0}] meshes reached", MAX_MESHES);

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

    void MeshRenderer::OnDrawUI()
    {
        GUI::BeginProperties();

        if (GUI::BeginTreeProperties("Material"))
        {
            GUI::FloatProperty("Shininess:", material.shininess, "Some tooltip.");

            GUI::FloatProperty("Vertex Exaggaration:", material.vertexExaggeration, "Some tooltip.");

            static const float resetValues[3] = { 1.0f, 1.0f, 1.0f };
            static const char* tooltips[3] = { "Some tooltip.", "Some tooltip.", "Some tooltip." };
            GUI::PropertyVector3("Diffuse:", material.diffuse, resetValues, tooltips);
            GUI::FloatProperty("Specular:", material.specular, "Some tooltip.");
            GUI::FloatProperty("Ambient:", material.ambient, "Some tooltip.");

            GUI::EndTreeProperties();
        }

        if (GUI::TextureProperty("Diffuse Texture:", textures[TextureType::DIFFUSE], "Some tooltip.")) SetTexture(textures[TextureType::DIFFUSE]);
        if (GUI::TextureProperty("Specular Texture:", textures[TextureType::SPECULAR], "Some tooltip.")) SetTexture(textures[TextureType::SPECULAR]);
        if (GUI::TextureProperty("Normal Map Texture:", textures[TextureType::NORMAL_MAP], "Some tooltip.")) SetTexture(textures[TextureType::NORMAL_MAP]);
        if (GUI::TextureProperty("Height Map Texture:", textures[TextureType::HEIGHT_MAP], "Some tooltip.")) SetTexture(textures[TextureType::HEIGHT_MAP]);

        GUI::EndProperties();
    }

    /* --- SETTER METHODS --- */

    void MeshRenderer::CreateDescriptorSet()
    {
        // If descriptor indexing not supported write default textures to the corresponding descriptor set
        descriptorSet = DescriptorSet::Create(VK::GetDescriptorSetLayout());
        descriptorSet->WriteTexture(DIFFUSE_TEXTURE_BINDING, Texture::GetDefaultTexture(TextureType::DIFFUSE));
        descriptorSet->WriteTexture(SPECULAR_TEXTURE_BINDING, Texture::GetDefaultTexture(TextureType::SPECULAR));
        descriptorSet->WriteTexture(NORMAL_MAP_TEXTURE_BINDING, Texture::GetDefaultTexture(TextureType::NORMAL_MAP));
        descriptorSet->WriteTexture(HEIGHT_MAP_TEXTURE_BINDING, Texture::GetDefaultTexture(TextureType::HEIGHT_MAP));
        descriptorSet->Allocate();
    }

    void MeshRenderer::SetTexture(const SharedPtr<Texture>& givenTexture)
    {
        ASSERT_ERROR_IF(givenTexture->GetTextureType() == TextureType::UNDEFINED_TEXTURE, "In order to bind texture to mesh its texture type must be specified and be different from TextureType::NONE");

        textures[givenTexture->GetTextureType()] = givenTexture;

        VK::GetDevice()->WaitUntilIdle();

        if (VK::GetDevice()->GetDescriptorIndexingSupported())
        {
            // TODO: BINDLESS
//            VulkanCore::GetGlobalBindlessDescriptorSet()->WriteTexture(BINDLESS_TEXTURE_BINDING, givenTexture, true, (TOTAL_TEXTURE_TYPES_COUNT * meshID) + static_cast<uint32_t>(givenTexture->GetTextureType()));
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
        ASSERT_ERROR_IF(textureType == TextureType::UNDEFINED_TEXTURE, "In order to reset a mesh's texture the texture type must not be TextureType::UNDEFINED_TEXTURE");

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
        return { .material = material, .entityID = static_cast<uint>(enttEntity), .meshID = meshID, .meshTexturesPresence =  meshTexturesPresence };
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
    }
}
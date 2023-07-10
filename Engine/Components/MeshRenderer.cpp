//
// Created by Nikolay Kanchevski on 20.11.22.
//

#include "MeshRenderer.h"

#include "Transform.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    using namespace Rendering;
    MeshRenderer::MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh)
        : mesh(std::move(givenCorrespondingMesh))
    {
        ASSERT_ERROR_FORMATTED_IF(IDPool.IsFull(), "Limit of maximum [{0}] meshes reached", MAX_MESHES);
        meshID = IDPool.CreateNewID();

        // Assign textures array to use default ones
        for (uint i = static_cast<uint>(TextureType::TOTAL_COUNT); i--;)
        {
            textures[i] = Texture::GetDefaultTexture(static_cast<TextureType>(i));
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

            GUI::EndTreeProperties();
        }

        if (GUI::TextureProperty("Diffuse Texture:", textures[static_cast<uint>(TextureType::DIFFUSE)], "Some tooltip.")) SetTexture(textures[static_cast<uint>(TextureType::DIFFUSE)]);
        if (GUI::TextureProperty("Specular Texture:", textures[static_cast<uint>(TextureType::SPECULAR)], "Some tooltip.")) SetTexture(textures[static_cast<uint>(TextureType::SPECULAR)]);
        if (GUI::TextureProperty("Normal Map Texture:", textures[static_cast<uint>(TextureType::NORMAL)], "Some tooltip.")) SetTexture(textures[static_cast<uint>(TextureType::NORMAL)]);
        if (GUI::TextureProperty("Height Map Texture:", textures[static_cast<uint>(TextureType::HEIGHT)], "Some tooltip.")) SetTexture(textures[static_cast<uint>(TextureType::HEIGHT)]);

        GUI::EndProperties();
    }

    /* --- SETTER METHODS --- */

    void MeshRenderer::SetTexture(const SharedPtr<Texture> &givenTexture)
    {
        ASSERT_ERROR_IF(givenTexture->GetTextureType() == TextureType::UNDEFINED, "In order to bind texture to mesh its texture type must be specified and be different from TextureType::NONE");
        textures[static_cast<uint>(givenTexture->GetTextureType())] = givenTexture;
        meshTexturesPresence.SetBit(static_cast<uint>(givenTexture->GetTextureType()), 1);
    }

    void MeshRenderer::ResetTexture(const TextureType textureType)
    {
        ASSERT_ERROR_IF(textureType == TextureType::UNDEFINED, "In order to reset a mesh's texture the texture type must not be TextureType::UNDEFINED_TEXTURE");
        SetTexture(Texture::GetDefaultTexture(textureType));
        meshTexturesPresence.SetBit(static_cast<uint>(textureType), 0);
    }

    /* --- GETTER METHODS --- */

    Matrix4x4 MeshRenderer::GetModelMatrix() const
    {
        return GetComponent<Transform>().GetModelMatrix();
    }

    MeshPushConstant MeshRenderer::GetPushConstantData() const
    {
        glm::normalize(material.diffuse);
        return { .material = material, .meshID = meshID, .entityID = static_cast<uint>(enttEntity), .meshTexturesPresence =  meshTexturesPresence };
    }

    /* --- DESTRUCTOR --- */

    void MeshRenderer::Destroy()
    {
        IDPool.RemoveID(meshID);

        // NOTE: We are not destroying textures as they must remain cached in texture pool

        mesh->Destroy();
    }
}
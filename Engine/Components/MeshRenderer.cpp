//
// Created by Nikolay Kanchevski on 20.11.22.
//

#include "MeshRenderer.h"

#include "Transform.h"
#include "../Classes/Math.h"
#include "../../Editor/GUI.h"

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

    void MeshRenderer::OnAddComponent()
    {
        auto &transform = GetComponent<Transform>();
        transform.PushOnDirtyCallback([this, &transform](const TransformDirtyFlag dirtyFlag){
            modelMatrix = Math::CreateModelMatrix(transform.GetWorldPosition(), transform.GetWorldRotation(), transform.GetWorldScale());
            normalMatrix = glm::transpose(glm::inverse(glm::mat3x3(modelMatrix)));
        });
    }

    /* --- POLLING METHODS --- */

    void MeshRenderer::OnDrawUI()
    {
        using namespace Sierra::Editor;
        GUI::BeginProperties();

        if (GUI::BeginTreeProperty("Material"))
        {
            GUI::FloatProperty("Shininess:", material.shininess, "Some tooltip.");

            GUI::FloatProperty("Vertex Exaggeration:", material.vertexExaggeration, "Some tooltip.");

            static const float resetValues[3] = { 1.0f, 1.0f, 1.0f };
            GUI::Vector3Property("Diffuse:", material.diffuse, "Some tooltip.", resetValues);
            GUI::FloatProperty("Specular:", material.specular, "Some tooltip.");

            GUI::EndTreeProperty();
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
        texturePresence.SetBit(static_cast<uint>(givenTexture->GetTextureType()), 1);
    }

    void MeshRenderer::ResetTexture(const TextureType textureType)
    {
        ASSERT_ERROR_IF(textureType == TextureType::UNDEFINED, "In order to reset a mesh's texture the texture type must not be TextureType::UNDEFINED_TEXTURE");
        SetTexture(Texture::GetDefaultTexture(textureType));
        texturePresence.SetBit(static_cast<uint>(textureType), 0);
    }

    /* --- DESTRUCTOR --- */

    void MeshRenderer::Destroy()
    {
        IDPool.RemoveID(meshID);

        // NOTE: We are not destroying textures as they must remain cached in texture pool

        mesh->Destroy();
    }
}
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
    MeshRenderer::MeshRenderer(UniquePtr<Mesh> &givenMesh, SharedPtr<Material> &material)
        : mesh(givenMesh), material(material)
    {
        ASSERT_ERROR_IF(IDPool.IsFull(), "Limit of maximum [{0}] meshes reached", MAX_MESHES);
        meshID = IDPool.CreateNewID();
    }

    void MeshRenderer::OnAddComponent()
    {
        auto &transform = GetComponent<Transform>();
        transform.PushOnDirtyCallback([this, &transform](const TransformDirtyFlag dirtyFlag) {
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
            GUI::FloatProperty("Shininess:", material->shininess, "Some tooltip.");

            GUI::FloatProperty("Vertex Exaggeration:", material->vertexExaggeration, "Some tooltip.");

            static const float resetValues[3] = { 1.0f, 1.0f, 1.0f };
            GUI::Vector3Property("Diffuse:", material->diffuse, "Some tooltip.", resetValues);
            GUI::FloatProperty("Specular:", material->specular, "Some tooltip.");

            GUI::EndTreeProperty();
        }

        // A very, very ugly way to say - "hey, reload that same texture"
        if (GUI::TextureProperty("Diffuse Texture:", material->GetTexture(TextureType::DIFFUSE), TextureType::DIFFUSE, "Some tooltip.")) material->ResetTexture(TextureType::DIFFUSE);
        if (GUI::TextureProperty("Specular Texture:", material->GetTexture(TextureType::SPECULAR), TextureType::SPECULAR, "Some tooltip.")) material->ResetTexture(TextureType::SPECULAR);
        if (GUI::TextureProperty("Normal Map Texture:", material->GetTexture(TextureType::NORMAL), TextureType::NORMAL, "Some tooltip.")) material->ResetTexture(TextureType::NORMAL);
        if (GUI::TextureProperty("Height Map Texture:", material->GetTexture(TextureType::HEIGHT), TextureType::HEIGHT, "Some tooltip.")) material->ResetTexture(TextureType::HEIGHT);

        GUI::EndProperties();
    }

    /* --- DESTRUCTOR --- */

    void MeshRenderer::Destroy()
    {
        IDPool.RemoveID(meshID);
    }
}
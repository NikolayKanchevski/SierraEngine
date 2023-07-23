//
// Created by Nikolay Kanchevski on some date lul.
//

#pragma once

#include "Component.h"

#include "../Classes/Mesh.h"
#include "../Classes/Binary.h"
#include "../Classes/Material.h"
#include "../Classes/IdentifierPool.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Texture.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Descriptors.h"

namespace Sierra::Engine
{

    class MeshRenderer : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        MeshRenderer() = default;
        MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh);
        void OnAddComponent() override;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- PROPERTIES --- */
        Material material{};

        /* --- SETTER METHODS --- */
        void SetTexture(const SharedPtr<Rendering::Texture> &givenTexture);
        void ResetTexture(Rendering::TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Mesh>& GetMesh() { return mesh; }
        [[nodiscard]] inline uint GetMeshID() const { return meshID; }

        [[nodiscard]] inline Matrix4x4 GetModelMatrix() const { return modelMatrix; };
        [[nodiscard]] inline Matrix3x3 GetNormalMatrix() const { return normalMatrix; };

        [[nodiscard]] inline Material& GetMaterial() { return material; };
        [[nodiscard]] inline Binary GetTexturePresence() const { return texturePresence; };
        [[nodiscard]] inline SharedPtr<Rendering::Texture> GetTexture(const Rendering::TextureType textureType) const { return textures[static_cast<uint>(textureType)]; }
        [[nodiscard]] inline SharedPtr<Rendering::Texture> *GetTextures() { return textures; }

        /* --- SETTER METHODS --- */
        void Destroy() override;

    private:
        SharedPtr<Mesh> mesh = nullptr;

        Matrix4x4 modelMatrix;
        Matrix3x3 normalMatrix;

        Binary texturePresence = 0; // A binary string of encoded bools, which indicate wether a given texture type is present
        SharedPtr<Rendering::Texture> textures[static_cast<uint>(Rendering::TextureType::TOTAL_COUNT)];

        uint meshID = 0;
        inline static auto IDPool = IdentifierPool<uint>(MAX_MESHES);
    };

}
//
// Created by Nikolay Kanchevski on some date lul.
//

#pragma once

#include "Component.h"

#include "../Classes/Mesh.h"
#include "../Classes/Binary.h"
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
        explicit MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh);

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
        [[nodiscard]] inline SharedPtr<Rendering::Texture> GetTexture(const Rendering::TextureType textureType) const { return textures[static_cast<uint>(textureType)]; }
        [[nodiscard]] inline SharedPtr<Rendering::Texture> *GetTextures() { return textures; }
        [[nodiscard]] Matrix4x4 GetModelMatrix() const;
        [[nodiscard]] MeshPushConstant GetPushConstantData() const;

        /* --- SETTER METHODS --- */
        void Destroy() override;

    private:
        SharedPtr<Mesh> mesh = nullptr;

        Binary meshTexturesPresence = 0;
        SharedPtr<Rendering::Texture> textures[static_cast<uint>(Rendering::TextureType::TOTAL_COUNT)];

        uint meshID = 0;
        inline static auto IDPool = IdentifierPool<uint>(MAX_MESHES);
    };

}
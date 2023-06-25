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

namespace Sierra::Engine::Components
{
    using namespace Sierra::Engine::Classes;

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
        void SetTexture(const SharedPtr<Texture>& givenTexture);
        void ResetTexture(TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Mesh>& GetMesh() { return mesh; }
        [[nodiscard]] inline uint GetMeshID() const { return meshID; }
        [[nodiscard]] inline SharedPtr<Texture> GetTexture(const TextureType textureType) const { return textures[static_cast<uint>(textureType)]; }
        [[nodiscard]] inline SharedPtr<Texture> *GetTextures() { return textures; }
        [[nodiscard]] Matrix4x4 GetModelMatrix() const;
        [[nodiscard]] MeshPushConstant GetPushConstantData() const;

        /* --- SETTER METHODS --- */
        void Destroy() override;

    private:
        SharedPtr<Mesh> mesh = nullptr;

        Binary meshTexturesPresence = 0;
        SharedPtr<Texture> textures[static_cast<uint>(TextureType::TOTAL_COUNT)];

        uint meshID = 0;
        inline static auto IDPool = IdentifierPool<uint>(MAX_MESHES);
    };

}
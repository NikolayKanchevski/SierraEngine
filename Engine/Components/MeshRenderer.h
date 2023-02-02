//
// Created by Nikolay Kanchevski on some date lul.
//

#pragma once

#include "Component.h"

#include "../../Core/Rendering/Vulkan/VK.h"
#include "../Classes/Mesh.h"
#include "../Classes/Binary.h"
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
        MeshRenderer(SharedPtr<Mesh> givenCorrespondingMesh);

        /* --- POLLING METHODS --- */
        void DrawUI() override;

        /* --- PROPERTIES --- */
        Material material {};

        /* --- SETTER METHODS --- */
        void SetTexture(const SharedPtr<Texture>& givenTexture);
        void ResetTexture(TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Mesh>& GetMesh() { return coreMesh; }
        [[nodiscard]] inline uint GetMeshID() const { return meshID; }
        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet->GetVulkanDescriptorSet(); }
        [[nodiscard]] inline SharedPtr<Texture> GetTexture(const TextureType textureType) const { return textures[textureType]; }
        [[nodiscard]] inline SharedPtr<Texture> *GetTextures() { return textures; }
        [[nodiscard]] Matrix4x4 GetModelMatrix() const;
        [[nodiscard]] MeshPushConstant GetPushConstantData() const;

        /* --- SETTER METHODS --- */
        void Destroy() const override;

    private:
        uint meshID;
        SharedPtr<Mesh> coreMesh = nullptr;

        SharedPtr<DescriptorSet> descriptorSet;
        SharedPtr<Texture> textures[TOTAL_TEXTURE_TYPES_COUNT];

        Binary meshTexturesPresence = 0;

        void CreateDescriptorSet();

        inline static uint meshSlotsUsed = 0;
        inline static std::vector<uint> freedMeshSlots;
    };

}
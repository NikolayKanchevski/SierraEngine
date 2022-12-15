//
// Created by Nikolay Kanchevski on some date lul.
//

#pragma once

#include <glm/mat4x4.hpp>

#include "Component.h"
#include "../Classes/Mesh.h"
#include "../Classes/Binary.h"
#include "../Structures/Material.h"
#include "../../Core/Rendering/Vulkan/Abstractions/Texture.h"

namespace Sierra::Engine::Components
{

    using namespace Sierra::Engine::Classes;

    class MeshRenderer : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        MeshRenderer() = default;
        MeshRenderer(std::shared_ptr<Mesh> givenCorrespondingMesh);

        /* --- PROPERTIES --- */
        Material material {};

        /* --- SETTER METHODS --- */
        void SetTexture(const std::shared_ptr<Texture>& givenTexture);
        void ResetTexture(TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline std::shared_ptr<Mesh>& GetMesh() { return coreMesh; }
        [[nodiscard]] inline uint32_t GetMeshID() const { return meshID; }
        [[nodiscard]] inline VkDescriptorSet GetDescriptorSet() const { return descriptorSet->GetVulkanDescriptorSet(); }
        [[nodiscard]] inline std::shared_ptr<Texture> GetTexture(const TextureType textureType) const { return textures[textureType]; }
        [[nodiscard]] inline std::shared_ptr<Texture> *GetTextures() { return textures; }
        [[nodiscard]] glm::mat4x4 GetModelMatrix() const;
        [[nodiscard]] PushConstant GetPushConstantData() const;

        /* --- SETTER METHODS --- */
        void Destroy() const override;

    private:
        uint32_t meshID;
        std::shared_ptr<Mesh> coreMesh = nullptr;

        std::shared_ptr<DescriptorSet> descriptorSet;
        std::shared_ptr<Texture> textures[TOTAL_TEXTURE_TYPES_COUNT];

        Binary meshTexturesPresence = 0;

        void CreateDescriptorSet();

        inline static uint32_t meshSlotsUsed = 0;
        inline static std::vector<uint32_t> freedMeshSlots;
    };

}
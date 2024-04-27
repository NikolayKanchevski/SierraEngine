//
// Created by Nikolay Kanchevski on 23.04.24.
//

#pragma once

#include "../Assets/Importers/TextureImporter.h"

namespace SierraEngine
{

    struct TextureCreateInfo
    {
        std::unique_ptr<Sierra::CommandBuffer> &commandBuffer;
        ImportedTexture&& importedTexture = { };
    };

    class Texture final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Texture(const TextureCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline TextureType GetType() const { return type; }
        [[nodiscard]] inline const std::unique_ptr<Sierra::Image>& GetImage() const { return image; }

        /* --- OPERATORS --- */
        Texture(const Texture&) = delete;
        Texture &operator=(const Texture&) = delete;

        /* --- MOVE SEMANTICS --- */
        Texture(Texture&& other) = default;
        Texture& operator=(Texture&& other) = default;

        /* --- DESTRUCTOR --- */
        ~Texture() = default;

    private:
        TextureType type = TextureType::Undefined;
        std::unique_ptr<Sierra::Image> image = nullptr;

    };

}
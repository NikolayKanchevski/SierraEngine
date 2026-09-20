//
// Created by Nikolay Kanchevski on 8.11.24.
//

#pragma once

namespace SierraEngine
{

    class glTFModelLoader final : public ModelLoader
    {
    public:
        /* --- CONSTRUCTORS --- */
        glTFModelLoader() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<LoadedModel> Load(const ModelLoadInfo& loadInfo) const override;

        /* --- COPY SEMANTICS --- */
        glTFModelLoader(const glTFModelLoader&) = delete;
        glTFModelLoader& operator=(const glTFModelLoader&) = delete;

        /* --- MOVE SEMANTICS --- */
        glTFModelLoader(glTFModelLoader&&) noexcept = default;
        glTFModelLoader& operator=(glTFModelLoader&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~glTFModelLoader() noexcept override = default;

    };

}
//
// Created by Nikolay Kanchevski on 30.12.24.
//

#pragma once

#include "../../Serializers/YAMLSerializer.h"

namespace SierraEngine
{

    class YAMLModelSerializer final : public ModelSerializer, public YAMLSerializer
    {
    public:
        /* --- CONSTRUCTORS --- */
        YAMLModelSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<SerializedModel> Serialize(const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outDependencies, ModelID& outID) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetSignature GetSignature() const noexcept override { return { 'Y', 'A', 'M', 'D' }; }
        [[nodiscard]] AssetVersion GetVersion() const noexcept override { return AssetVersion({ 1, 0, 0 }); }

        /* --- COPY SEMANTICS --- */
        YAMLModelSerializer(const YAMLModelSerializer&) = delete;
        YAMLModelSerializer& operator=(const YAMLModelSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        YAMLModelSerializer(YAMLModelSerializer&&) noexcept = default;
        YAMLModelSerializer& operator=(YAMLModelSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~YAMLModelSerializer() noexcept override = default;

    };

}
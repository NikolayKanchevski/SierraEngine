//
// Created by Nikolay Kanchevski on 23.04.24.
//

#pragma once

#include "../../../SerializedScene.h"
#include "../../../../Core/Scene.h"

namespace SierraEngine
{

    struct SceneSerializerCreateInfo
    {

    };

    struct SceneSerializeInfo
    {
        const Scene &scene;
    };

    class SceneSerializer final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit SceneSerializer(const SceneSerializerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::vector<uint8> Serialize(const SceneSerializeInfo &serializeInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return VERSION; }

        /* --- OPERATORS --- */
        SceneSerializer(const SceneSerializer&) = delete;
        SceneSerializer &operator=(const SceneSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~SceneSerializer() = default;

    private:
        constexpr static Sierra::Version VERSION = Sierra::Version({ 1, 0, 0 });

        template<typename... Component>
        [[nodiscard]] uint32 SerializeEntityComponents(entt::entity entity, const entt::registry &registry, ComponentGroup<Component...>, std::vector<uint8> &blob, uint64 &seekOffset);

    };

}

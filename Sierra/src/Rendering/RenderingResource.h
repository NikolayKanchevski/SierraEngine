//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingBackendType.h"

namespace Sierra
{

    class SIERRA_API RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetName() const noexcept = 0;
        [[nodiscard]] virtual RenderingBackendType GetBackendType() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        RenderingResource(const RenderingResource&) = delete;
        RenderingResource& operator=(const RenderingResource&) = delete;

        /* --- DESTRUCTORS --- */
        virtual ~RenderingResource() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        RenderingResource() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        RenderingResource(RenderingResource&&) noexcept = default;
        RenderingResource& operator=(RenderingResource&&) noexcept = default;

    };

    /* --- CONCEPTS --- */
    template<typename T>
    concept RenderingResourceType = std::is_base_of_v<RenderingResource, T> && !std::is_same_v<RenderingResource, T>;

}

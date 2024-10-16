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

        /* --- MOVE SEMANTICS --- */
        RenderingResource(RenderingResource&&) = delete;
        RenderingResource& operator=(RenderingResource&&) = delete;

        /* --- DESTRUCTORS --- */
        virtual ~RenderingResource() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        RenderingResource() noexcept = default;

    };

}

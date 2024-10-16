//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "../RenderingContext.h"
#include "MetalResource.h"

namespace Sierra
{

    class SIERRA_API MetalContext final : public RenderingContext, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalContext(const RenderingContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Device> CreateDevice(const DeviceCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }
        [[nodiscard]] Version GetBackendVersion() const noexcept override { return metalVersion; }

        /* --- COPY SEMANTICS --- */
        MetalContext(const MetalContext&) = delete;
        MetalContext& operator=(const MetalContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalContext(MetalContext&&) = delete;
        MetalContext& operator=(MetalContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalContext() noexcept override = default;

    private:
        const std::string name;
        Version metalVersion = Version({ 0, 0, 0 });

    };

}

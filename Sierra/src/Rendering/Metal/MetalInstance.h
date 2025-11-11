//
// Created by Nikolay Kanchevski on 21.11.23.
//

#pragma once

#include "MetalResource.h"
#include "../RenderingInstance.h"

namespace Sierra
{

    class SIERRA_API MetalInstance final : public RenderingInstance, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MetalInstance(const RenderingInstanceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Device> CreateDevice(const DeviceCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Version GetBackendVersion() const noexcept override { return metalVersion; }

        /* --- COPY SEMANTICS --- */
        MetalInstance(const MetalInstance&) = delete;
        MetalInstance& operator=(const MetalInstance&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalInstance(MetalInstance&&) noexcept = default;
        MetalInstance& operator=(MetalInstance&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MetalInstance() noexcept override = default;

    private:
        Version metalVersion = Version({ 0, 0, 0 });

    };

}

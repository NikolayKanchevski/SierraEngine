//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Core/Version.h"

#include "Device.h"

namespace Sierra
{

    struct RenderingContextCreateInfo
    {
        std::string_view name = "Rendering Context";
        std::string_view applicationName = "Sierra Application";
        Version applicationVersion = Version({ 1, 0, 0 });
    };

    class SIERRA_API RenderingContext : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Device> CreateDevice(const DeviceCreateInfo& createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual Version GetBackendVersion() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        RenderingContext(const RenderingContext&) = delete;
        RenderingContext& operator=(const RenderingContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        RenderingContext(RenderingContext&&) = delete;
        RenderingContext& operator=(RenderingContext&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~RenderingContext() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit RenderingContext(const RenderingContextCreateInfo& createInfo);

    };

}

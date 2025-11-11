//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Core/Version.h"

#include "Device.h"

namespace Sierra
{

    struct RenderingInstanceCreateInfo
    {
        std::string_view name = "Rendering Context";
        std::string_view applicationName = "Sierra Application";
        Version applicationVersion = Version({ 1, 0, 0 });
    };

    class SIERRA_API RenderingInstance : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Device> CreateDevice(const DeviceCreateInfo& createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual Version GetBackendVersion() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        RenderingInstance(const RenderingInstance&) = delete;
        RenderingInstance& operator=(const RenderingInstance&) = delete;

        /* --- MOVE SEMANTICS --- */
        RenderingInstance(RenderingInstance&&) noexcept = default;
        RenderingInstance& operator=(RenderingInstance&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        virtual ~RenderingInstance() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit RenderingInstance(const RenderingInstanceCreateInfo& createInfo);

    };

}

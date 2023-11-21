//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"
#include "Device.h"

namespace Sierra
{

    struct RenderingContextCreateInfo
    {
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    class SIERRA_API RenderingContext : public virtual RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Device& GetDevice() const = 0;

    protected:
        explicit RenderingContext(const RenderingContextCreateInfo &createInfo);

    private:
        friend class Application;
        static UniquePtr<RenderingContext> Create(const RenderingContextCreateInfo &createInfo);

        void Destroy() override;

    };

}

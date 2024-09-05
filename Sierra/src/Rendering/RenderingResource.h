//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

namespace Sierra
{

    #if SR_METAL_SUPPORTED
        #define GRAPHICS_API_AUTO Metal
    #elif SR_VULKAN_SUPPORTED
        #define GRAPHICS_API_AUTO Vulkan
    #elif SR_DIRECTX_SUPPORTED
        #define GRAPHICS_API_AUTO DirectX
    #elif SR_OPENGL_SUPPORTED
        #define GRAPHICS_API_AUTO OpenGL
    #else
        #define GRAPHICS_API_AUTO Undefined
    #endif

    enum class GraphicsAPI : uint8
    {
        Undefined,
        Vulkan,
        DirectX,
        Metal,
        OpenGL,
        Auto = GRAPHICS_API_AUTO
    };

    #undef GRAPHICS_API_AUTO

    class SIERRA_API RenderingResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetName() const = 0;
        [[nodiscard]] virtual GraphicsAPI GetAPI() const = 0;

        /* --- COPY SEMANTICS --- */
        RenderingResource(const RenderingResource&) = delete;
        RenderingResource& operator=(const RenderingResource&) = delete;

        /* --- MOVE SEMANTICS --- */
        RenderingResource(RenderingResource&&) = default;
        RenderingResource& operator=(RenderingResource&&) = default;

        /* --- DESTRUCTORS --- */
        virtual ~RenderingResource() = default;

    protected:
        RenderingResource() = default;

    };

}

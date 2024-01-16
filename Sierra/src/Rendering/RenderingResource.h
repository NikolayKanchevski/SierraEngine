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
        #if SR_ENABLE_LOGGING
            [[nodiscard]] inline const std::string& GetName() const { return name; }
        #else
            [[nodiscard]] inline const std::string& GetName() const { static std::string name = ""; return name; }
        #endif
        [[nodiscard]] inline virtual GraphicsAPI GetAPI() const = 0;

    protected:
        #if SR_ENABLE_LOGGING
            std::string name;
        #endif
        RenderingResource() = default;

    };

}

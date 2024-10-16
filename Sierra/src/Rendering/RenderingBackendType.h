//
// Created by Nikolay Kanchevski on 8.10.24.
//

#pragma once

namespace Sierra
{

    enum class RenderingBackendType : uint8
    {
        Vulkan,
        DirectX,
        Metal,
        OpenGL,
        WebGPU,
        #if SR_METAL_SUPPORTED
            Best = Metal
        #elif SR_VULKAN_SUPPORTED
            Best = Vulkan
        #elif SSR_DIRECTX_SUPPORTED
            Best = DirectX
        #elif SSR_OPENGL_SUPPORTED
            Best = OpenGL
        #elif SSR_WEBGPU_SUPPORTED
            Best = OpenGL
        #else
            Best = Undefined
        #endif
    };

}
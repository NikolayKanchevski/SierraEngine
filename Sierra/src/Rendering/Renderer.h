//
// Created by Nikolay Kanchevski on 24.06.24.
//

#pragma once

#include "CommandBuffer.h"
#include "Image.h"

namespace Sierra
{

    class SIERRA_API Renderer
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Resize(uint32 width, uint32 height) = 0;
        virtual void Render(CommandBuffer& commandBuffer, const Image& outputImage) = 0;

        /* --- COPY SEMANTICS --- */
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        /* --- MOVE SEMANTICS --- */
        Renderer(Renderer&&) = default;
        Renderer& operator=(Renderer&&) = default;
        
        /* --- DESTRUCTOR --- */
        virtual ~Renderer() = default;
    
    protected:
        Renderer() = default;
    
    };

}
